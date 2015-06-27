#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "dwebsvr.h"

// embedded resource files
#include "code.h"
#include "index.h"
#include "jquery-2-1-0-min.h"
#include "smoothie.h"

void* polling_thread(void *args);
void send_response(struct hitArgs *args, char*, char*, http_verb);
void log_filter(log_type, char*, char*, int);
void send_entropy_response(struct hitArgs *args, char*, char*);
int path_ends_with(char *path, char *match);
void send_file_response(struct hitArgs *args, char*, char*, int);
int get_entropy();

int entropy;
pthread_t polling_thread_id;

int main(int argc, char **argv)
{
	if (argc < 2 || !strncmp(argv[1], "-h", 2))
	{
		printf("hint: eWeb [port number]\n");
		return 0;
	}

    if (pthread_create(&polling_thread_id, NULL, polling_thread, NULL) !=0)
    {
        fprintf(stderr, "Error: pthread_create could not create polling thread");
        exit(EXIT_FAILURE);
    }
    
    // don't read from the console or log anything
    dwebserver(atoi(argv[1]), &send_response, &log_filter);
    
    return 1; // just to stop compiler complaints
}

void* polling_thread(void *args)
{
    struct timespec ts;
    ts.tv_sec = 0;
    ts.tv_nsec = 250 * 1000000; // 250 ms
    while (1)
    {
        entropy = get_entropy();
        nanosleep(&ts, NULL);
    }
    return NULL;
}

void log_filter(log_type type, char *s1, char *s2, int socket_fd)
{
    // log to null :-)
}

// decide if we need to send an API response or a file...
void send_response(struct hitArgs *args, char *path, char *request_body, http_verb type)
{
	int path_length=(int)strlen(path);
	
	if (path_ends_with(path, "entropy.api"))
	{
		return send_entropy_response(args, path, request_body);
	}
    
	send_file_response(args, path, request_body, path_length);
}

// returns the entropy
void send_entropy_response(struct hitArgs *args, char *path, char *request_body)
{
    char tmp[13];
    STRING *response = new_string(32);
    
    if (entropy >= 0)
    {
        sprintf(tmp, "%d", entropy);
    }
    else
    {
        sprintf(tmp, "0");
    }
    
    string_add(response, tmp);
    ok_200(args, "\nContent-Type: text/plain", string_chars(response), path);
    string_free(response);
}

int get_entropy()
{
#ifdef __APPLE__
    return rand() % 100;
#else
    FILE *fp = fopen ("/proc/sys/kernel/random/entropy_avail", "r");
    if (fp != NULL)
    {
        int e = 0;
        fscanf(fp, "%d", &e);
        fclose (fp);
        return e;
    }
    else
    {
        return 0;
    }
#endif
}

int path_ends_with(char *path, char *match)
{
    int match_len = (int)strlen(match);
    int path_length = (int)strlen(path);
    return (path_length >= match_len && !strncmp(&path[path_length-match_len], match, match_len));
}

void send_file_response(struct hitArgs *args, char *path, char *request_body, int path_length)
{
    STRING *response = new_string(1024);
    string_add(response, "HTTP/1.1 200 OK\n");
    string_add(response, "Connection: close\n");
    string_add(response, "Content-Type: ");
    
	if (!strcmp(path, "") || path_ends_with(path, "index.html"))
    {
        string_add(response, "text/html");
        write_header(args->socketfd, string_chars(response), index_html_len);
        write(args->socketfd, index_html, index_html_len);
    }
    else if (path_ends_with(path, "code.js"))
    {
        string_add(response, "text/javascript");
        write_header(args->socketfd, string_chars(response), code_js_len);
        write(args->socketfd, code_js, code_js_len);
    }
    else if (path_ends_with(path, "smoothie.js"))
    {
        string_add(response, "text/javascript");
        write_header(args->socketfd, string_chars(response), smoothie_js_len);
        write(args->socketfd, smoothie_js, smoothie_js_len);
    }
    else if (path_ends_with(path, "jquery-2-1-0-min.js"))
    {
        string_add(response, "text/javascript");
        write_header(args->socketfd, string_chars(response), jquery_2_1_0_min_js_len);
        write(args->socketfd, jquery_2_1_0_min_js, jquery_2_1_0_min_js_len);
    }
    else
    {
        notfound_404(args, "no such file");
    }
    
    string_free(response);
    
    // allow socket to drain before closing
    sleep(1);
}
