#include "minishell.h"

int is_valid_var_char(char c)
{
    return (ft_isalnum(c) || c == '_');
}

char *realloc_result(char *result, int *max_size, int needed)
{
    char *new_result;

    if (needed <= *max_size)
        return (result);
    
    *max_size = needed + 100;
    new_result = realloc(result, *max_size);
    if (!new_result)
    {
        free(result);
        return (NULL);
    }
    return (new_result);
}

int copy_var_value(char **res, int *len, int *max, char *val)
{
    int vlen;
    char *temp;

    if (!val)
        val = "";
    
    vlen = strlen(val);
    temp = realloc_result(*res, max, *len + vlen + 1);
    if (!temp)
        return (0);
    
    *res = temp;
    strcpy(*res + *len, val);
    *len += vlen;
    return (1);
}

char *extract_var_name(char *str, int start, int *end)
{
    int i;

    if (!str || !end)
        return (NULL);
    
    i = start;
    if (str[i] == '?')
    {
        *end = i + 1;
        return (ft_strdup("?"));
    }
    
    while (str[i] && is_valid_var_char(str[i]))
        i++;
    
    *end = i;
    if (i == start)
        return (NULL);
    
    return (ft_strndup(str + start, i - start));
}

char *expand_exit_status(int exit_code)
{
    return (ft_itoa(exit_code));
}

// Updated to take env_list parameter
int handle_special_var(char *name, int exit_code, char **value, t_env *env_list)
{
    if (!name || !value)
        return (0);
    
    if (strcmp(name, "?") == 0)
    {
        *value = expand_exit_status(exit_code);
        return (1); // indicates we allocated memory for value
    }
    
    *value = get_env_value(env_list, name);
    return (0); // indicates value points to existing memory
}

void cleanup_var_expansion(char *name, char *value, int is_special)
{
    if (name)
        free(name);
    if (value && is_special)
        free(value);
}

int process_regular_char(char *content, int *i, t_expand_data *data)
{
    char *temp;
    
    if (!content || !i || !data || !data->res || !data->len || !data->max)
        return (0);
    
    temp = realloc_result(*(data->res), data->max, *(data->len) + 2);
    if (!temp)
        return (0);
    
    *(data->res) = temp;
    (*(data->res))[*(data->len)] = content[*i];
    (*(data->len))++;
    (*i)++;
    
    return (1);
}

// Updated to use env_list from data structure
int process_dollar_expansion(char *content, int *i, t_expand_data *data)
{
    int var_end;
    char *name;
    char *value;
    int is_special;

    if (!content || !i || !data)
        return (0);

    name = extract_var_name(content, *i, &var_end);
    if (name)
    {
        is_special = handle_special_var(name, data->exit_code, &value, data->env_list);
        if (!copy_var_value(data->res, data->len, data->max, value))
        {
            cleanup_var_expansion(name, value, is_special);
            return (0);
        }
        cleanup_var_expansion(name, value, is_special);
        *i = var_end;
        return (1);
    }
    
    // If we couldn't extract a variable name, just copy the '$' literally
    if (!process_regular_char("$", &(int){0}, data))
        return (0);
    
    return (1);
}

int process_expansion_loop(char *content, t_expand_data *data)
{
    int i;

    if (!content || !data)
        return (0);
    
    i = 0;
    while (content[i])
    {
        if (content[i] == '$')
        {
            i++;
            if (!process_dollar_expansion(content, &i, data))
                return (0);
        }
        else if (!process_regular_char(content, &i, data))
            return (0);
    }
    return (1);
}

// Updated to take env_list parameter
char *expand_token_content(char *content, int exit_code, int should_expand, t_env *env_list)
{
    char *res;
    int len;
    int max;
    t_expand_data data;

    if (!content)
        return (NULL);
    
    if (!should_expand)
        return (ft_strdup(content));
    
    len = 0;
    max = 1024;
    res = malloc(max);
    if (!res)
        return (NULL);
    
    data.res = &res;
    data.len = &len;
    data.max = &max;
    data.exit_code = exit_code;
    data.env_list = env_list;
    
    if (!process_expansion_loop(content, &data))
    {
        free(res);
        return (NULL);
    }
    
    res[len] = '\0';
    return (res);
}

// Updated to take env_list parameter
void handle_word_token(t_elem *curr, int exit_code, t_env *env_list)
{
    int should_expand;
    char *exp;

    if (!curr || !curr->content)
        return;
    
    // NEVER expand inside a single-quoted token
    should_expand = (curr->state != IN_QUOTE);
    
    exp = expand_token_content(curr->content, exit_code, should_expand, env_list);
    if (exp)
    {
        free(curr->content);
        curr->content = exp;
        curr->type = WORD;
        // keep curr->state unchanged – it may still be IN_DQUOTE or GENERAL
    }
}

char *remove_quotes(char *content, enum e_type quote_type)
{
    int len;
    char quote_char;

    if (!content)
        return (NULL);
    
    len = strlen(content);
    if (len < 2)
        return (ft_strdup(content));
    
    if (quote_type == QUOTE)
        quote_char = '\'';
    else if (quote_type == DQUOTE)
        quote_char = '"';
    else
        return (ft_strdup(content));
    
    if (content[0] == quote_char && content[len - 1] == quote_char)
        return (ft_strndup(content + 1, len - 2));
    
    return (ft_strdup(content));
}

// Updated to take env_list parameter
void handle_quoted_token(t_elem *curr, int exit_code, t_env *env_list)
{
    char *unquoted;
    char *expanded;
    int should_expand;

    if (!curr || !curr->content)
        return;
    
    unquoted = remove_quotes(curr->content, curr->type);
    if (!unquoted)
        return;
    
    // Only expand inside double quotes, not single quotes
    should_expand = (curr->type == DQUOTE);
    
    expanded = expand_token_content(unquoted, exit_code, should_expand, env_list);
    free(unquoted);
    
    if (expanded)
    {
        free(curr->content);
        curr->content = expanded;
        curr->type = WORD;
    }
}

// Updated to take env_list parameter
void expand_tokens(t_elem *token, int exit_code, t_env *env_list)
{
    t_elem *curr;

    if (!token)
        return;
    
    curr = token;
    while (curr)
    {
        if (curr->type == QUOTE || curr->type == DQUOTE)
            handle_quoted_token(curr, exit_code, env_list);
        else if ((curr->type == WORD || curr->type == ENV) 
                 && curr->state != IN_QUOTE)
            handle_word_token(curr, exit_code, env_list);
        curr = curr->next;
    }
}