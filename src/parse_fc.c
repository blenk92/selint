#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "parse_fc.h"
#include "tree.h"

// "gen_context("
#define GEN_CONTEXT_LEN 12

struct fc_entry * parse_fc_line(char *line) {
	char whitespace[] = " \t";

	struct fc_entry *out = malloc(sizeof(struct fc_entry));
	memset(out, 0, sizeof(struct fc_entry));

	char *orig_line = strdup(line); // If the object class is ommitted, we need to revert

	char *pos = strtok(line, whitespace);

	out->path = strdup(pos);

	pos = strtok(NULL, whitespace);

	if (pos == NULL) {
		printf("Here 1\n");
		goto cleanup;
	}

	if (pos[0] == '-') {
		if ( pos[2] != '\0') {
			printf("Here 2\n");
			goto cleanup;
		}
		out->obj = pos[1];
		pos = strtok(NULL, whitespace);
		if (pos == NULL ) {
			printf("Here 3\n");
			goto cleanup;
		}
	}

	// pos points to the start of the context, but spaces in the context may have been
	// overwitten by strtok
	strcpy(line, orig_line);

	if (strncmp("gen_context(", pos, GEN_CONTEXT_LEN) == 0) {
		pos += GEN_CONTEXT_LEN; // Next character
		char *context_part = strtok(pos, ",");
		printf("%s\n", context_part);
		if (context_part == NULL) {
			printf("Here 4\n");
			goto cleanup;
		}

		char *tmp = strtok(NULL,",");
		if ( tmp == NULL ) {
			printf("Here 5\n");
			goto cleanup;
		}
		int i = 0;
		while (tmp[i] != '\0' && tmp[i] != ')') {
			i++;
		}
		if (tmp[i] == '\0') {
			// Missing closing paren
			printf("Here 6\n");
			goto cleanup;
		}
		tmp[i] = '\0';
		while (tmp[0] != '\0' && (tmp[0] == ' ' || tmp[0] == '\t')) {
			// trim beginning whitespace
			tmp++;
		}

		out->context = parse_context(context_part);
		if (out ->context == NULL) {
			goto cleanup;
		}
		out->context->has_gen_context = 1;
		out->context->range = strdup(tmp);
	} else {
		out->context = parse_context(pos);
		if (out->context == NULL) {
			goto cleanup;
		}
		out->context->has_gen_context = 0;
	} 

	if (out->context == NULL) {
		printf("here 7\n");
		goto cleanup;
	}

	free(orig_line);
	return out;

cleanup:
	free(orig_line);
	free_fc_entry(out);
	return NULL;
}

struct sel_context * parse_context(char *context_str) {

	struct sel_context *context = malloc(sizeof(struct sel_context));
	memset(context, 0, sizeof(struct sel_context));
	// User
	char *pos = strtok(context_str, ":");

	if (pos == NULL) {
		goto cleanup;
	}

	context->user = strdup(pos);

	// Role
	pos = strtok(NULL, ":");

	if (pos == NULL) {
		goto cleanup;
	}

	context->role = strdup(pos);

	// Type
	pos = strtok(NULL, ":");

	if (pos == NULL) {
		goto cleanup;
	}

	context->type = strdup(pos);

	pos = strtok(NULL, ":");

	if (pos) {
		context->range = strdup(pos);
		if(strtok(NULL, ":")) {
			goto cleanup;
		}
	}

	return context;

cleanup:
	free_sel_context(context);
	return NULL;
}

struct policy_node * parse_fc_file(char *filename) {
	return NULL;
}

void free_fc_entry(struct fc_entry *to_free) {
	if (to_free->path) {
		free(to_free->path);
	}
	if (to_free->context) {
		free_sel_context(to_free->context);
	}
	free(to_free);
}

void free_sel_context(struct sel_context *to_free) {
	if (to_free->user) {
		free(to_free->user);
	}
	if (to_free->role) {
		free(to_free->role);
	}
	if (to_free->type) {
		free(to_free->type);
	}
	if (to_free->range) {
		free(to_free->range);
	}
	free(to_free);
}