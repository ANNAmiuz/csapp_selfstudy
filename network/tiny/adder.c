#include "csapp.h"

int main(void)
{
    char * buf, *p, *method;
    char arg1[MAXLINE], arg2[MAXLINE], content[MAXLINE];
    int n1 = 0, n2 = 0;

    /* get the two arguments */
    method = getenv("REQUEST_METHOD");
    if ((buf = getenv("QUERY_STRING")) != NULL){
        p = strchr(buf, '&');
        *p = '\0';
        strcpy(arg1, buf);
        strcpy(arg2, p+1);
        n1 = atoi(arg1);
        n2 = atoi(arg2);
    }

    /* response body */
    sprintf(content, "QUERY_STRING=%s", buf);
    sprintf(content, "Welcome to add.com: ");
    sprintf(content, "%sThe Internet addition portal. \r\n<p>", content);
    sprintf(content, "%sThe answer is %d + %d = %d\r\n", content, n1, n2, n1+n2);
    sprintf(content, "%sThanks for visiting!\r\n", content);

    /* HTTP response*/
    printf("Connection: close\r\n");
    printf("Content-length: %d\r\n", (int)strlen(content));
    printf("Content-type: text/html\r\n\r\n");
    if (strcasecmp(method, "HEAD") != 0)
        printf("%s", content);
    fflush(1);
    exit(0);
}