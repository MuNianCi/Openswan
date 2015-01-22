#define LEAK_DETECTIVE
#define AGGRESSIVE 1
#define XAUTH 1
#define PRINT_SA_DEBUG 1

#include <stdlib.h>
#include "openswan.h"
#include "openswan/ipsec_policy.h"
#include "oswalloc.h"
#include "constants.h"
#include "certs.h"
#include "oswlog.h"
#include "oswconf.h"
#include "oswtime.h"

#include "../seam_exitlog.c"

char *progname;

/** by default pluto does not check crls dynamically */
long crl_check_interval = 0;
struct pubkey_list *pluto_pubkeys = NULL;
struct secret *pluto_secrets = NULL;

main(int argc, char *argv[])
{
    int i;
    chunk_t blob, crl_uri;
    err_t e;
    cert_t cacert,t1;
    time_t until;

    progname = argv[0];
    leak_detective=1;

    tool_init_log();
    load_oswcrypto();

    set_debugging(DBG_X509|DBG_PARSING);
    until =1421896274;
    set_fake_x509_time(until);  /* Wed Jan 21 22:11:14 2015 */

    if(argc < 3) {
        fprintf(stderr, "Usage: nsscert CAcertfile.pem cert1.pem cert2.pem...\n");
        exit(5);
    }

    /* skip argv0 */
    argc--;
    argv++;

    /* load CAcert */
    if(!load_cert(CERT_NONE, argv[0], TRUE, "cacert", &cacert)) {
        printf("could not load CA cert file: %s\n", argv[0]);
        exit(1);
    }
    add_authcert(cacert.u.x509, AUTH_CA);

    argc--;
    argv++;

    while(argc-- > 0) {
        char *file = *argv++;
        /* load target cert */
        if(!load_cert(CERT_NONE, file, TRUE, "test1", &t1)) {
            printf("could not load cert file: %s\n", file);
            exit(1);
        }


        until += 86400;
        if(verify_x509cert(t1.u.x509, TRUE, &until) == FALSE) {
            printf("verify x509 failed\n");
            exit(3);
        }
        printf("cert: %s is valid\n", file);
        free_x509cert(t1.u.x509);
    }
    free_x509cert(cacert.u.x509);

    report_leaks();
    tool_close_log();
    exit(0);
}

/*
 * Local Variables:
 * c-style: pluto
 * c-basic-offset: 4
 * End:
 */
