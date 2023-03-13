![Greenbone Logo](https://www.greenbone.net/wp-content/uploads/gb_logo_resilience_horizontal.png) ![Boreas Logo](doc/img/Boreas_AliveScanner_horizontal.png)

# Boreas

Boreas is a command line tool to scan for alive hosts. It supports IPv4 and IPv6 address ranges and allows to exclude certain addresses from a range. The alive ping tests support ICMP, TCP-ACK, TCP-SYN and ARP and any combination. For TCP ping an individual port list can be applied.

## Releases

All [release files](https://github.com/greenbone/boreas/releases) are signed with
the [Greenbone Community Feed integrity key](https://community.greenbone.net/t/gcf-managing-the-digital-signatures/101).
This gpg key can be downloaded at https://www.greenbone.net/GBCommunitySigningKey.asc
and the fingerprint is `8AE4 BE42 9B60 A59B 311C  2E73 9823 FAA6 0ED1 E580`.

## Installation

This module can be configured, built and installed with following commands:

    cmake .
    make install

For detailed installation requirements and instructions, please see the file
[INSTALL.md](INSTALL.md).

## Example

```bash
$ sudo boreas --target "127.0.0.1-10" --exclude "127.0.0.5" --ports "80" --icmp --tcp-ack
```

For a detailed description of all available options see the manpage.
For a short list of available options use the `-h` command.

## Support

For any question on the usage of `boreas` please use the [Greenbone
Community Portal](https://community.greenbone.net/c/gse). If you found a problem
with the software, please [create an
issue](https://github.com/greenbone/boreas/issues) on GitHub. If you
are a Greenbone customer you may alternatively or additionally forward your
issue to the Greenbone Support Portal.

## Maintainer

This project is maintained by [Greenbone AG](https://www.greenbone.net/).

## Contributing

Your contributions are highly appreciated. Please [create a pull
request](https://github.com/greenbone/boreas/pulls) on GitHub. Bigger
changes need to be discussed with the development team via the [issues section
at GitHub](https://github.com/greenbone/boreas/issues) first.

### Code style and formatting

Before creating a pull request, it is recommended to run the following command:

    make format

This reformats the new code to ensure that it follows the code style and
formatting guidelines.

## License

Copyright (C) 2020 [Greenbone AG](https://www.greenbone.net/)

Licensed under the [GNU General Public License v3.0 or later](LICENSE).
