/* Copyright (C) 2020 Greenbone AG
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <errno.h>
#include <gvm/base/logging.h>
#include <gvm/base/networking.h>
#include <gvm/base/version.h> /* for gvm_libs_version */
#include <gvm/boreas/alivedetection.h>
#include <gvm/boreas/cli.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>

#undef G_LOG_DOMAIN
/**
 * @brief GLib log domain.
 */
#define G_LOG_DOMAIN "alive scan"

/**
 * @brief Logging parameters, as passed to setup_log_handlers.
 */
GSList *log_config = NULL;

/**
 * @brief Start an alive scan as specified by the provided commandline options.
 *
 * @param argc Argument count.
 * @param argv Argument vector.
 */
int
main (int argc, char *argv[])
{
  alive_test_t alive_test;
  GSList *unresolved;
  gvm_hosts_t *hosts;

  int err;
  gchar *default_port_list;
  static gchar *rc_name = NULL;
  static gboolean display_version = FALSE;
  static gchar *host_list = NULL;
  static gchar *exclude_list = NULL;
  static gchar *port_list = NULL;
  static gboolean icmp = FALSE;
  static gboolean tcp_ack = FALSE;
  static gboolean tcp_syn = FALSE;
  static gboolean arp = FALSE;
  static unsigned int wait_timeout = 3;

  GError *error = NULL;
  GOptionContext *option_context;
  static GOptionEntry entries[] = {
    {"version", 'v', 0, G_OPTION_ARG_NONE, &display_version,
     "Display version information.", NULL},
    {"target", '\0', 0, G_OPTION_ARG_STRING, &host_list,
     "List of hosts to test.", "<string>"},
    {"exclude", '\0', 0, G_OPTION_ARG_STRING, &exclude_list,
     "List of hosts to exclude from target list.", "<string>"},
    {"ports", '\0', 0, G_OPTION_ARG_STRING, &port_list,
     "List of ports to test. Default port list is \"80,137,587,3128,8081\".",
     "<string>"},
    {"icmp", '\0', 0, G_OPTION_ARG_NONE, &icmp,
     "ICMP ping. Default method when no method specified. Supports both IPv4 "
     "and IPv6.",
     NULL},
    {"tcp-syn", '\0', 0, G_OPTION_ARG_NONE, &tcp_syn,
     "TCP-SYN ping. Supports both IPv4 and IPv6.", NULL},
    {"tcp-ack", '\0', 0, G_OPTION_ARG_NONE, &tcp_ack,
     "TCP-ACK ping. Supports both IPv4 and IPv6.", NULL},
    {"arp", '\0', 0, G_OPTION_ARG_NONE, &arp,
     "ARP ping. Supports both IPv4 and IPv6.", NULL},
    {"timeout", '\0', 0, G_OPTION_ARG_INT, &wait_timeout,
     "Wait time for replies.", NULL},
    {NULL, 0, 0, 0, NULL, NULL, NULL}};

  option_context = g_option_context_new ("- Boreas");
  g_option_context_add_main_entries (option_context, entries, NULL);
  if (!g_option_context_parse (option_context, &argc, &argv, &error))
    {
      g_print ("%s\n\n", error->message);
      exit (0);
    }
  g_option_context_free (option_context);

  /* --version */
  if (display_version)
    {
      printf ("Boreas %s\n", BOREAS_VERSION);
      printf ("gvm-libs %s\n", gvm_libs_version ());
      printf ("(C) 2020 Greenbone AG\n");
      printf ("License GPLv3+: GNU GPL version 3 or later\n");
      printf (
        "This is free software: you are free to change and redistribute it.\n"
        "There is NO WARRANTY, to the extent permitted by law.\n\n");
      exit (0);
    }

  if (setenv ("TZ", "utc 0", 1) == -1)
    {
      g_print ("%s\n\n", strerror (errno));
      exit (0);
    }
  tzset ();

  rc_name = g_build_filename (BOREAS_SYSCONF_DIR, "boreas_log.conf", NULL);
  if (g_file_test (rc_name, G_FILE_TEST_EXISTS))
    log_config = load_log_configuration (rc_name);
  g_free (rc_name);
  setup_log_handlers (log_config);

  if (geteuid ())
    {
      printf ("Warning: Boreas needs to be run as root. \n");
      exit (0);
    }

  /* Create alive test bit flag.*/
  alive_test =
    (icmp ? ALIVE_TEST_ICMP : 0) | (tcp_ack ? ALIVE_TEST_TCP_ACK_SERVICE : 0)
    | (tcp_syn ? ALIVE_TEST_TCP_SYN_SERVICE : 0) | (arp ? ALIVE_TEST_ARP : 0);
  /* Use ICMP ping as default if there was no method specified. */
  if (alive_test == 0)
    alive_test = ALIVE_TEST_ICMP;

    /* Verify that the port list is a valid one or use default portlist if none
     * supplied. */
#ifdef DEFAULT_PORT_LIST
  default_port_list = DEFAULT_PORT_LIST;
#else
  /* TODO: the hardcoded default port list and the preprocessor directives
   * must be removed once 22.4 reaches its end of life. This is done for
   * backward compatibility. */
  default_port_list = "80,137,587,3128,8081";
#endif

  if (tcp_syn || tcp_ack)
    {
      if (port_list)
        {
          printf ("%s", port_list);
          if (validate_port_range (port_list))
            {
              printf (
                "Invalid port list. Ports must be in the range [1-65535]. \n");
              return -1;
            }
        }
      else
        {
          port_list = default_port_list;
        }
    }

  /* Create host list. */
  hosts = gvm_hosts_new (host_list);
  if (NULL == hosts)
    {
      printf ("Host list is not Valid.\n");
      return -1;
    }
  unresolved = gvm_hosts_resolve (hosts);
  while (unresolved)
    {
      printf ("Couldn't resolve hostname '%s'\n", (char *) unresolved->data);
      unresolved = unresolved->next;
    }
  g_slist_free_full (unresolved, g_free);

  /* Exclude hosts from host list. */
  if (exclude_list)
    {
      /* Exclude hosts, resolving hostnames. */
      int ret = gvm_hosts_exclude (hosts, exclude_list);
      if (ret > 0)
        printf ("exclude_list: Skipped %d host(s).\n", ret);
      if (ret < 0)
        printf ("Exclude host list is not Valid.\n");
    }

  /* Run the cli scan. */
  err = run_cli_extended (hosts, alive_test, port_list, wait_timeout);
  if (err)
    printf ("Could not run the scan. %s. Further information can be found in "
            "the log file located in %s. \n",
            str_boreas_error (err), BOREAS_LOG_DIR);

  return 0;
}
