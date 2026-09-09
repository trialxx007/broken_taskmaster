#include "../../../include/taskmaster.h"
#include "../../../include/config.h"
#include "../../../include/utils.h"
#include <yaml.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
/******************************************************************************/
/* */
/* ÉTAPE 1 : VALIDATION GLOBALE DU FICHIER ET DES BLOCS               */
/* */
/******************************************************************************/

/* * ÉTAPE 1.1 : Vérification du dictionnaire principal
 * - S'assurer que le nœud racine contient la clé obligatoire "programs".
 * - Vérifier que "programs" est bien un dictionnaire/objet (Map).
 */

/* * ÉTAPE 1.2 : Boucle de vérification des noms de programmes
 * - Pour chaque clé trouvée sous "programs" (ex: "nginx", "vogsphere") :
 * a) Vérifier que la clé n'est pas vide et ne dépasse pas une taille max (ex: 255 car.).
 * b) Vérifier que le nom n'est pas un doublon dans notre structure de stockage.
 */


/******************************************************************************/
/* */
/* ÉTAPE 2 : VALIDATION DES PARAMÈTRES (POUR CHAQUE PROGRAMME)        */
/* */
/******************************************************************************/

/* * ÉTAPE 2.1 : Validation de "cmd" (Obligatoire)
 * - Vérifier la présence de la clé "cmd".
 * - Vérifier que le type est une chaîne de caractères (String).
 * - Vérifier que la chaîne n'est pas vide ("").
 * - Optionnel mais recommandé : Isoler le premier mot (l'exécutable) et vérifier 
 * via 'access(path, X_OK)' si le fichier existe et est exécutable.
 */

/* * ÉTAPE 2.2 : Validation de "numprocs" (Optionnel, défaut = 1)
 * - Si présent, vérifier que le type est un entier (Integer).
 * - Vérifier la limite basse : numprocs doit être STRICTEMENT supérieur à 0 (> 0).
 * - Vérifier la limite haute (sécurité Fork-Bomb) : numprocs <= MAX_PROCS (ex: 100).
 */

/* * ÉTAPE 2.3 : Validation de "umask" (Optionnel, défaut = 022)
 * - Si présent, récupérer la valeur STRICTEMENT sous forme de chaîne (String).
 * - Utiliser 'strtol(str, &endptr, 8)' pour forcer la lecture en base 8 (octal).
 * - Vérifier la validité de la chaîne :
 * a) Si endptr == str ou *endptr != '\0', la chaîne contient des caractères invalides.
 * b) Si errno == ERANGE, il y a eu un overflow.
 * - Vérifier les limites numériques : la valeur finale doit être >= 0 et <= 511 (0777 octal).
 */

/* * ÉTAPE 2.4 : Validation de "workingdir" (Optionnel)
 * - Si présent, vérifier que c'est une chaîne de caractères.
 * - Vérifier que le chemin commence par '/' (chemin absolu).
 * - Utiliser 'access(path, F_OK | X_OK)' pour s'assurer que le dossier existe 
 * et que le superviseur a le droit d'y entrer ('chdir').
 */

/* * ÉTAPE 2.5 : Validation de "autostart" (Optionnel, défaut = true)
 * - Si présent, vérifier que le type est un booléen (True/False).
 */

/* * ÉTAPE 2.6 : Validation de "autorestart" (Optionnel, défaut = unexpected)
 * - Si présent, vérifier que c'est soit un booléen, soit une chaîne de caractères.
 * - Si c'est une chaîne, vérifier qu'elle correspond STRICTEMENT à l'une de ces 
 * trois valeurs : "true", "false", ou "unexpected". Tout autre mot est une erreur.
 */

/* * ÉTAPE 2.7 : Validation de "exitcodes" (Optionnel, défaut = 0)
 * - Gérer la flexibilité du format (Polymorphisme du parseur) :
 * a) Si c'est un entier unique : vérifier qu'il est >= 0 et <= 255.
 * b) Si c'est une liste/tableau : boucler sur chaque élément et vérifier 
 * que chaque entier est >= 0 et <= 255.
 * - Stocker le résultat final dans un tableau d'entiers en mémoire.
 */

/* * ÉTAPE 2.8 : Validation de "startretries" (Optionnel, défaut = 3)
 * - Si présent, vérifier que c'est un entier.
 * - Vérifier que la valeur est >= 0.
 */

/* * ÉTAPE 2.9 : Validation de "starttime" (Optionnel, défaut = 1)
 * - Si présent, vérifier que c'est un entier (secondes).
 * - Vérifier que la valeur est >= 0.
 */

/* * ÉTAPE 2.10 : Validation de "stopsignal" (Optionnel, défaut = TERM)
 * - Si présent, vérifier que c'est une chaîne de caractères.
 * - Comparer la chaîne avec la liste des signaux UNIX connus (ex: "TERM", "INT", "KILL", "USR1").
 * - Convertir la chaîne validée en son équivalent entier macro C (ex: "TERM" -> SIGTERM).
 * - Sécurité : Interdire ou lever un warning si le signal est SIGKILL (9) ou SIGSTOP (19) 
 * car ils ne peuvent pas être interceptés par l'enfant.
 */

/* * ÉTAPE 2.11 : Validation de "stoptime" (Optionnel, défaut = 10)
 * - Si présent, vérifier que c'est un entier (secondes).
 * - Vérifier que la valeur est >= 0.
 */

/* * ÉTAPE 2.12 : Validation de "stdout" et "stderr" (Optionnels)
 * - Si présents, vérifier que ce sont des chaînes de caractères (chemins de fichiers).
 * - Pour chaque fichier :
 * a) Extraire le chemin du dossier parent (ex: de "/tmp/nginx.stdout" extraire "/tmp").
 * b) Vérifier via 'access(parent_dir, W_OK)' que le superviseur a le droit 
 * d'écrire dans ce dossier pour pouvoir y créer ou ouvrir le fichier de log.
 */

/* * ÉTAPE 2.13 : Validation de "env" (Optionnel)
 * - Si présent, vérifier que c'est un dictionnaire/objet (Clé-Valeur).
 * - Pour chaque paire Clé:Valeur :
 * a) Vérifier que la Clé ne contient pas de signe '=' et respecte la nomenclature 
 * des variables d'environnement (lettres, chiffres, underscores).
 * b) Convertir de force la Valeur en chaîne de caractères (String), même si c'est 
 * un entier (ex: 42 -> "42"), car 'execve' requiert un tableau de chaînes.
 */


/******************************************************************************/
/* */
/* ÉTAPE 3 : FINALISATION ET ALLOCATION                               */
/* */
/******************************************************************************/

/* * ÉTAPE 3.1 : Application des valeurs par défaut
 * - Si un paramètre optionnel est absent du fichier de configuration, 
 * lui assigner sa valeur par défaut dans la structure de données avant de clore le parsing.
 */

/* * ÉTAPE 3.2 : Validation finale de la structure
 * - Si TOUTES les étapes précédentes sont valides -> Retourner 1 (Succès), le superviseur peut démarrer.
 * - Si UNE SEULE étape a échoué -> Libérer proprement toute la mémoire déjà allouée, 
 * afficher un message d'erreur précis sur le canal 'stderr' et retourner 0 (Échec).
 */
 
 /******************************************************************************/
/* */
/* ÉTAPE 1 : VALIDATION GLOBALE DU FICHIER ET DES BLOCS                       */
/* */
/******************************************************************************/

/* ÉTAPE 1.1 : "error: missing mandatory 'programs' root key" */
/* ÉTAPE 1.1 : "error: 'programs' key must be a dictionary" */

/* ÉTAPE 1.2 : "error: program name cannot be empty" */
/* ÉTAPE 1.2 : "error: program name exceeds maximum length" */
/* ÉTAPE 1.2 : "error: duplicate program name '%s' found" */


/******************************************************************************/
/* */
/* ÉTAPE 2 : VALIDATION DES PARAMÈTRES (POUR CHAQUE PROGRAMME)                */
/* */
/******************************************************************************/

/* ÉTAPE 2.1 : "error: [%s] missing mandatory 'cmd' key" */
/* ÉTAPE 2.1 : "error: [%s] 'cmd' must be a string" */
/* ÉTAPE 2.1 : "error: [%s] 'cmd' cannot be empty" */
/* ÉTAPE 2.1 : "error: [%s] executable '%s' not found or not executable" */

/* ÉTAPE 2.2 : "error: [%s] 'numprocs' must be an integer" */
/* ÉTAPE 2.2 : "error: [%s] 'numprocs' must be strictly greater than 0" */
/* ÉTAPE 2.2 : "error: [%s] 'numprocs' exceeds maximum allowed limit" */

/* ÉTAPE 2.3 : "error: [%s] 'umask' must be a string" */
/* ÉTAPE 2.3 : "error: [%s] 'umask' contains invalid octal characters" */
/* ÉTAPE 2.3 : "error: [%s] 'umask' value out of range (must be between 000 and 0777)" */

/* ÉTAPE 2.4 : "error: [%s] 'workingdir' must be a string" */
/* ÉTAPE 2.4 : "error: [%s] 'workingdir' must be an absolute path" */
/* ÉTAPE 2.4 : "error: [%s] working directory '%s' does not exist or is inaccessible" */

/* ÉTAPE 2.5 : "error: [%s] 'autostart' must be a boolean" */

/* ÉTAPE 2.6 : "error: [%s] 'autorestart' must be a boolean or a string" */
/* ÉTAPE 2.6 : "error: [%s] invalid 'autorestart' value (expected: true, false, unexpected)" */

/* ÉTAPE 2.7 : "error: [%s] 'exitcodes' must be an integer or a list of integers" */
/* ÉTAPE 2.7 : "error: [%s] invalid 'exitcodes' value %d (must be between 0 and 255)" */

/* ÉTAPE 2.8 : "error: [%s] 'startretries' must be an integer" */
/* ÉTAPE 2.8 : "error: [%s] 'startretries' cannot be negative" */

/* ÉTAPE 2.9 : "error: [%s] 'starttime' must be an integer" */
/* ÉTAPE 2.9 : "error: [%s] 'starttime' cannot be negative" */

/* ÉTAPE 2.10 : "error: [%s] 'stopsignal' must be a string" */
/* ÉTAPE 2.10 : "error: [%s] unknown or invalid signal '%s'" */
/* ÉTAPE 2.10 : "error: [%s] signal '%s' cannot be caught or ignored" */

/* ÉTAPE 2.11 : "error: [%s] 'stoptime' must be an integer" */
/* ÉTAPE 2.11 : "error: [%s] 'stoptime' cannot be negative" */

/* ÉTAPE 2.12 : "error: [%s] 'stdout' path must be a string" */
/* ÉTAPE 2.12 : "error: [%s] 'stderr' path must be a string" */
/* ÉTAPE 2.12 : "error: [%s] cannot write to log directory '%s'" */

/* ÉTAPE 2.13 : "error: [%s] 'env' must be a dictionary" */
/* ÉTAPE 2.13 : "error: [%s] invalid environment variable name '%s' (cannot contain '=')" */


/******************************************************************************/
/* */
/* ÉTAPE 3 : FINALISATION                                                     */
/* */
/******************************************************************************/

/* ÉTAPE 3.2 : "error: configuration parsing failed, clearing memory" */

t_config	*config_parse(const char *filename)
{
	t_config		*config;
	yaml_parser_t	parser;
	t_parse_context	context;
	FILE			*file;

	config = config_create();
	memset(&parser, ZERO, sizeof(yaml_parser_t));
	context.parser = &parser;
	file = NULL;
	if (!config)
		return (NULL);
	if (!config_open_file(filename, &parser, &file))
	{
		free_config(config);
		return (NULL);
	}
	if (!config_parse_events(config, &context))
	{
		free_config(config);
		config_close_file(&parser, &file);
		return (NULL);
	}
	config_close_file(&parser, &file);
	return (config);
}

void	handle_struct_events(t_parse_context* context)
{
	if (context == NULL)
		return ;
	switch (context->event.type)
	{
		case YAML_MAPPING_START_EVENT:
			context->is_key = true;
			context->depth++;
			break ;
		case YAML_MAPPING_END_EVENT:
			context->is_key = true;
			context->depth--;
			break ;
		case YAML_SEQUENCE_START_EVENT:
			context->in_sequence = true;
			context->is_key = false;
			break ;
		case YAML_SEQUENCE_END_EVENT:
			context->in_sequence = false;
			context->is_key = true;
			break ;
		default:
			break ;
	}
}

static bool	handle_scalar_key(t_config* conf, t_parse_context* context, const char* val)
{
	if (context == NULL || conf == NULL || val == NULL)
		return (false);
	if (context->depth == FIRST_DEPTH)
	{
		if (strcmp(val, "programs") != ZERO)
		{
			write_error_fmt("error: root key must be 'programs'.\n");
			return (false);
		}
	}
	else if (context->depth == SECOND_DEPTH)
	{
		if (!config_parse_program(conf, val))
			return (false);
		context->is_key = false;
	}
	else if (context->depth == THIRD_DEPTH || context->depth == MAX_DEPTH)
	{
		free(context->current_key);
		context->current_key = strdup(val);
		if (context->current_key == NULL)
			return (false);
		context->is_key = false;
	}
	return (true);
}

static bool	handle_scalar_value(t_config* conf, t_parse_context* context, const char* val)
{
	t_program_config*	curr_program;

	curr_program = (conf->programs_count > ZERO) ?
		&conf->programs[(conf->programs_count - 1)] : NULL;
	if (curr_program == NULL || context == NULL || conf == NULL || val == NULL)
	{
		write_error_fmt("error: config file is corrupted.\n");
		return (false);
	}
	if (context->depth == THIRD_DEPTH && context->in_sequence == false)
	{
		if (context->current_key && strcmp(context->current_key, "exitcodes") == ZERO)
		{
			char*	endptr;
			long	result;
			result = strtol(val, &endptr, DECIMAL);
			if (endptr == val || *endptr != '\0' || result < ZERO || result > MAX_UCHAR)
			{
				write_error_fmt("error: exitcodes must be valid integers between %d and %d.\n", ZERO, MAX_UCHAR);
				return (false);
			}
			config_add_exitcode(curr_program, (int)result);
			context->is_key = true;
			return (true);
		}
		if (!config_set_field(curr_program, context->current_key, val))
			return (false);
		context->is_key = true;
	}
	else if (context->depth == MAX_DEPTH)
	{
		if (!context->current_key
			|| !parse_block_env(curr_program, context->current_key, val))
			return (false);
		context->is_key = true;
	}
	else if (context->in_sequence == true)
	{
		if (context->current_key && strcmp(context->current_key, "exitcodes") == ZERO)
		{
			char*	endptr;
			long	result;
			result = strtol(val, &endptr, DECIMAL);
			if (endptr == val || *endptr != '\0' || result < ZERO || result > MAX_UCHAR)
			{
				write_error_fmt("error: exitcodes must be valid integers between %d and %d.\n", ZERO, MAX_UCHAR);
				return (false);
			}
			config_add_exitcode(curr_program, (int)result);
		}
		else
		{
			write_error_fmt("error: unexpected sequence structure or invalid key for list.\n");
			return (false);
		}
	}
	else
	{
		write_error_fmt("error: unexpected configuration value structure.\n");
		return (false);
	}
	return (true);
}

bool	parse_scalar_event(t_config* conf, t_parse_context* context, const char* val)
{
	if (context == NULL || conf == NULL || val == NULL)
		return (false);
	if (context->is_key)
		return (handle_scalar_key(conf, context, val));
	return (handle_scalar_value(conf, context, val));
}

bool	config_parse_events(t_config* config, t_parse_context* context)
{
	if (context == NULL || config == NULL)
		return (false);
	init_parse_event_struct(context);
	while (!context->done)
	{
		if (yaml_parser_parse(context->parser, &context->event) == ZERO)
		{
			write_error_fmt("error: failed to parse yaml file.\n");
			context->is_no_problem = false;
			break ;
		}
		context->done = (context->event.type == YAML_STREAM_END_EVENT);
		handle_struct_events(context);
		if (context->event.type == YAML_SCALAR_EVENT)
		{
			if (!parse_scalar_event(config, context,
				(char *)context->event.data.scalar.value))
				context->is_no_problem = false;
		}
		yaml_event_delete(&context->event);
		if (context->is_no_problem == false) break ;
	}
	free(context->current_key);
	context->current_key = NULL;
	free(context->env_key);
	context->env_key = NULL;
	return (context->is_no_problem);
}

bool	config_parse_program(t_config *config, const char *name)
{
	t_program_config	*tmp;
	char*				alloc_name;

	if (config == NULL || name == NULL || *name == '\0' || strlen(name) > MAX_SIZE)
	{
		write_error_fmt("error: program name is invalid or too long.\n");
		return (false);
	}
	for (int i = ZERO; name[i] != '\0'; ++i)
	{
		if (isspace((unsigned char)name[i]))
		{
			write_error_fmt("error: program name contains invalid spaces.\n");
			return (false);
		}
	}
	for (int i = ZERO; i < config->programs_count; i++)
	{
		if (config->programs[i].name
			&& strcmp(config->programs[i].name, name) == ZERO)
		{
			write_error_fmt("error: duplicate program name '%s' found in configuration file.\n", config->programs->name);
			return (false);
		}
	}
	alloc_name = strdup(name);
	if (alloc_name == NULL)
	{
		write_error_fmt("error: memory allocation failed.\n");
		return (false);
	}
	tmp = realloc(config->programs,
		(config->programs_count + 1) * sizeof(t_program_config));
	if (tmp == NULL)
	{
		free(alloc_name);
		write_error_fmt("error: memory allocation failed.\n");
		return (false);
	}
	config->programs = tmp;
	config->programs_count++;
	init_program_config(&config->programs[(config->programs_count - 1)]);
	config->programs[(config->programs_count - 1)].name = alloc_name;
	return (true);
}

static bool	validate_cmd(const char* val, t_program_config* curr_program)
{
	if (curr_program->cmd != NULL)
	{
		write_error_fmt("error: duplicate key 'cmd' in program '%s'.\n", curr_program->name);
		return (false);
	}
	if (val == NULL || *val == '\0')
	{
		write_error_fmt("error: cmd cannot be empty.\n");
		return (false);
	}
	return (true);
}

static bool	validate_numprocs(const char* val, int* res, t_program_config* curr_program)
{
	char*	endptr;
	long	result;

	if (curr_program->numprocs != ZERO)
	{
		write_error_fmt("error: duplicate key 'numprocs' in program '%s'.\n", curr_program->name);
		return (false);
	}
	result = strtol(val, &endptr, DECIMAL);
	if (endptr == val || *endptr != '\0' || result < 1 || result > INT_MAX)
	{
		write_error_fmt("error: numprocs must be a valid strictly positive integer.\n");
		return (false);
	}
	*res = (int)result;
	return (true);
}

static bool	validate_umask(const char* val, mode_t* res, t_program_config* curr_program)
{
	char*	endptr;
	long	result;

	if (curr_program->umask != ZERO)
	{
		write_error_fmt("error: duplicate key 'umask' in program '%s'.\n", curr_program->name);
		return (false);
	}
	result = strtol(val, &endptr, OCTAL);
	if (endptr == val || *endptr != '\0' || result < ZERO || result > MAX_UMASK)
	{
		write_error_fmt("error: umask must be a valid octal value (0000 to 0777).\n");
		return (false);
	}
	*res = (mode_t)result;
	return (true);
}

static bool	validate_startretries(const char* val, int* res, const char* field, t_program_config* curr_program)
{
	char*	endptr;
	long	result;

	if (curr_program->startretries != ZERO)
	{
		write_error_fmt("error: duplicate key 'startretries' in program '%s'.\n", curr_program->name);
		return (false);
	}
	result = strtol(val, &endptr, DECIMAL);
	if (endptr == val || *endptr != '\0' || result < ZERO || result > INT_MAX)
	{
		write_error_fmt("error: %s must be a positive integer.\n", field);
		return (false);
	}
	*res = (int)result;
	return (true);
}

static bool	validate_starttime(const char* val, int* res, const char* field, t_program_config* curr_program)
{
	char*	endptr;
	long	result;

	if (curr_program->starttime != ZERO)
	{
		write_error_fmt("error: duplicate key 'starttime' in program '%s'.\n", curr_program->name);
		return (false);
	}
	result = strtol(val, &endptr, DECIMAL);
	if (endptr == val || *endptr != '\0' || result < ZERO || result > INT_MAX)
	{
		write_error_fmt("error: %s must be a positive integer.\n", field);
		return (false);
	}
	*res = (int)result;
	return (true);
}

static bool	validate_stoptime(const char* val, int* res, const char* field, t_program_config* curr_program)
{
	char*	endptr;
	long	result;

	if (curr_program->stoptime != ZERO)
	{
		write_error_fmt("error: duplicate key 'stoptime' in program '%s'.\n", curr_program->name);
		return (false);
	}
	result = strtol(val, &endptr, DECIMAL);
	if (endptr == val || *endptr != '\0' || result < ZERO || result > INT_MAX)
	{
		write_error_fmt("error: %s must be a positive integer.\n", field);
		return (false);
	}
	*res = (int)result;
	return (true);
}

static bool	validate_autostart(const char* val, bool* res, t_program_config* curr_program)
{
	if (curr_program->has_autostart)
	{
		write_error_fmt("error: duplicate key 'autostart' in program '%s'.\n", curr_program->name);
		return (false);
	}
	if (strcmp(val, "true") == ZERO)
	{
		*res = true;
		curr_program->has_autostart = true;
		return (true);
	}
	if (strcmp(val, "false") == ZERO)
	{
		*res = false;
		curr_program->has_autostart = true;
		return (true);
	}
	write_error_fmt("error: autostart must be 'true' or 'false'.\n");
	return (false);
}

static bool	validate_autorestart(const char* val, t_autorestart *res, t_program_config* curr_program)
{
	if (curr_program->has_autorestart)
	{
		write_error_fmt("error: duplicate key 'autorestart' in program '%s'.\n", curr_program->name);
		return (false);
	}
	if (strcmp(val, "always") == ZERO)
	{
		*res = AUTORESTART_ALWAYS;
		curr_program->has_autorestart = true;
		return (true);
	}
	if (strcmp(val, "never") == ZERO)
	{
		*res = AUTORESTART_NEVER;
		curr_program->has_autorestart = true;
		return (true);
	}
	if (strcmp(val, "unexpected") == ZERO)
	{
		*res = AUTORESTART_UNEXPECTED;
		curr_program->has_autorestart = true;
		return (true);
	}
	write_error_fmt("error: invalid autorestart value '%s'.\n", val);
	return (false);
}

static bool	validate_stopsignal(const char* val, int* res, t_program_config* curr_program)
{
	int	sig;

	if (curr_program->has_stopsignal)
	{
		write_error_fmt("error: duplicate key 'stopsignal' in program '%s'.\n", curr_program->name);
		return (false);
	}
	sig = signal_from_string(val);
	if (sig == -1)
	{
		write_error_fmt("error: invalid signal '%s'.\n", val);
		return (false);
	}
	*res = sig;
	curr_program->has_stopsignal = true;
	return (true);
}

bool	config_set_field(t_program_config *program, const char *key, const char *value)
{
	int				tmp_int;
	mode_t			tmp_mode;
	bool			tmp_bool;
	t_autorestart	tmp_restart;

	if (key == NULL || value == NULL)
	{
		write_error_fmt("error: configuration key or value cannot be (null).\n");
		return (false);
	}
	if (strcmp(key, "cmd") == ZERO)
	{
		if (!validate_cmd(value, program))
			return (false);
		free(program->cmd);
		program->cmd = strdup(value);
		if (program->cmd == NULL)
		{
			write_error_fmt("error: memory allocation failed.\n");
			return (false);
		}
	}
	else if (strcmp(key, "numprocs") == ZERO)
	{
		if (!validate_numprocs(value, &tmp_int, program))
			return (false);
		program->numprocs = tmp_int;
	}
	else if (strcmp(key, "umask") == ZERO)
	{
		if (!validate_umask(value, &tmp_mode, program))
			return (false);
		program->umask = tmp_mode;
	}
	else if (strcmp(key, "autostart") == ZERO)
	{
		if (!validate_autostart(value, &tmp_bool, program))
			return (false);
		program->autostart = tmp_bool;
	}
	else if (strcmp(key, "autorestart") == ZERO)
	{
		if (!validate_autorestart(value, &tmp_restart, program))
			return (false);
		program->autorestart = tmp_restart;
	}
	else if (strcmp(key, "stopsignal") == ZERO)
	{
		if (!validate_stopsignal(value, &tmp_int, program))
			return (false);
		program->stopsignal = tmp_int;
	}
	else if (strcmp(key, "startretries") == ZERO)
	{
		if (!validate_startretries(value, &tmp_int, "startretries", program))
			return (false);
		program->startretries = tmp_int;
	}
	else if (strcmp(key, "starttime") == ZERO)
	{
		if (!validate_starttime(value, &tmp_int, "starttime", program))
			return (false);
		program->starttime = tmp_int;
	}
	else if (strcmp(key, "stoptime") == ZERO)
	{
		if (!validate_stoptime(value, &tmp_int, "stoptime", program))
			return (false);
		program->stoptime = tmp_int;
	}
	else if (strcmp(key, "workingdir") == ZERO)
	{
		if (program->workingdir != NULL)
		{
			write_error_fmt("error: duplicate key 'workingdir' in program '%s'.\n", program->name);
			return (false);
		}
		free(program->workingdir);
		program->workingdir = strdup(value);
		if (program->workingdir == NULL)
		{
			write_error_fmt("error: memory allocation failed.\n");
			return (false);
		}
	}
	else if (strcmp(key, "stdout") == ZERO)
	{
		if (program->std_out != NULL)
		{
			write_error_fmt("error: duplicate key 'stdout' in program '%s'.\n", program->name);
			return (false);
		}
		free(program->std_out);
		program->std_out = strdup(value);
		if (program->std_out == NULL)
		{
			write_error_fmt("error: memory allocation failed.\n");
			return (false);
		}
	}
	else if (strcmp(key, "stderr") == ZERO)
	{
		if (program->std_err != NULL)
		{
			write_error_fmt("error: duplicate key 'stderr' in program '%s'.\n", program->name);
			return (false);
		}
		free(program->std_err);
		program->std_err = strdup(value);
		if (program->std_err == NULL)
		{
			write_error_fmt("error: memory allocation failed.\n");
			return (false);
		}
	}
	else
	{
		write_error_fmt("error: unknown configuration key '%s'.\n", key);
		return (false);
	}
	return (true);
}

int	signal_from_string(const char *value)
{
	if (value == NULL)
		return (TSK_ERROR);
	if (strcmp(value, "HUP") == 0) return (SIGHUP);
	if (strcmp(value, "INT") == 0) return (SIGINT);
	if (strcmp(value, "QUIT") == 0) return (SIGQUIT);
	if (strcmp(value, "KILL") == 0) return (SIGKILL);
	if (strcmp(value, "USR1") == 0) return (SIGUSR1);
	if (strcmp(value, "USR2") == 0) return (SIGUSR2);
	if (strcmp(value, "TERM") == 0) return (SIGTERM);
	return (TSK_ERROR);
}

t_config	*config_create(void)
{
	t_config	*config;

	config = malloc(sizeof(t_config));
	if (config == NULL)
	{
		write_error_fmt("error: failed to allocate memory.\n");
		return (NULL);
	}
	init_config(config);
	return (config);
}

bool	config_open_file(const char *filename, yaml_parser_t *parser, FILE **file)
{
	*file = fopen(filename, "r");
	if (*file == NULL)
	{
		perror(filename);
		return (false);
	}
	if (!yaml_parser_initialize(parser))
	{
		write_error_fmt("error: failed to initialize yaml parser.\n");
		fclose(*file);
		return (false);
	}
	yaml_parser_set_input_file(parser, *file);
	return (true);
}

void	config_close_file(yaml_parser_t *parser, FILE **file)
{
	yaml_parser_delete(parser);
	fclose(*file);
}
