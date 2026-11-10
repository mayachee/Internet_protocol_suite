# ft_traceroute

`ft_traceroute` is a C reimplementation of the Unix `traceroute` command. The
program discovers the route taken by IPv4 packets to a destination by sending
probes with increasing IP time-to-live (TTL) values and displaying the routers
that respond.

This project is part of the UNIX project series and is based on version 5.0 of
the provided subject.

## Netzwerkgrundlagen

### FQDN und DNS

Ein **FQDN** (Fully Qualified Domain Name) ist ein vollständiger Hostname,
zum Beispiel `www.google.com` oder `en.wikipedia.org`. Ein FQDN besteht aus
mehreren Teilen, die durch Punkte getrennt sind:

```text
www.google.com
│   │      └── Top-Level-Domain (TLD)
│   └───────── Domain
└───────────── Hostname/Subdomain
```

`ft_traceroute` akzeptiert sowohl eine numerische IPv4-Adresse als auch einen
Hostname oder FQDN:

```sh
sudo ./ft_traceroute 8.8.8.8
sudo ./ft_traceroute google.com
sudo ./ft_traceroute www.wikipedia.org
```

Bei einem Namen verwendet das Programm `getaddrinfo()`, um den Namen einmal
vor dem Start der Messung in eine IPv4-Adresse aufzulösen. Diese sogenannte
Forward-DNS-Auflösung ist notwendig, damit die Probe gesendet werden kann.

Für die einzelnen Router wird dagegen keine Reverse-DNS-Auflösung ausgeführt.
Die Hop-Adressen werden direkt numerisch mit `inet_ntop()` ausgegeben. Dadurch
vermeidet das Programm zusätzliche DNS-Anfragen und die Ausgabe bleibt schnell
und eindeutig:

```text
1  192.168.1.1  1.234 ms
```

### IPv4 und TTL

IPv4-Pakete enthalten im Header das Feld **TTL** (Time To Live). TTL ist kein
direkter Zeitwert in Sekunden, sondern ein Zähler für die maximale Anzahl von
Router-Hops. Jeder Router verringert den TTL-Wert um mindestens eins, bevor er
das Paket weiterleitet.

`ft_traceroute` sendet für jeden Hop neue Probes mit einem steigenden TTL-Wert:

```text
TTL = 1  -> erster Router
TTL = 2  -> zweiter Router
TTL = 3  -> dritter Router
```

Erreicht der TTL-Wert eines Pakets null, verwirft der Router das Paket und
sendet eine ICMP-Fehlermeldung zurück. Aus der Quelladresse dieser Meldung
erkennt `ft_traceroute`, welcher Router auf diesem Hop liegt.

Die Laufzeit einer Probe wird als **Round-Trip Time (RTT)** gemessen. Sie
beginnt beim Senden der Probe und endet beim Empfang der passenden ICMP-Antwort.

### ICMP

**ICMP** (Internet Control Message Protocol) wird direkt über IPv4 übertragen
und dient vor allem für Kontroll- und Fehlermeldungen. `ft_traceroute` nutzt
einen Raw Socket:

```c
socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)
```

Die wichtigsten ICMP-Nachrichten für dieses Projekt sind:

| Nachricht | Typ | Bedeutung |
| --- | ---: | --- |
| Echo Request | 8 | Die von `ft_traceroute` gesendete Probe |
| Echo Reply | 0 | Das Ziel hat die Probe erreicht |
| Time Exceeded | 11 | Ein Router hat die Probe wegen TTL `0` verworfen |
| Destination Unreachable | 3 | Das Ziel oder ein Router meldet, dass die Zustellung nicht möglich ist |

Eine ICMP-Fehlermeldung enthält normalerweise den ursprünglichen IPv4-Header
und einen Teil des ursprünglichen ICMP-Pakets. Das Programm prüft darin
Identifier und Sequenznummer, damit Antworten fremder Prozesse ignoriert
werden.

### Ablauf einer Traceroute-Messung

Für jeden Hop läuft die Messung grundsätzlich so ab:

1. Das Ziel wird mit `getaddrinfo()` in eine IPv4-Adresse umgewandelt.
2. `ft_traceroute` setzt den TTL des Raw-Sockets auf den aktuellen Hop.
3. Eine ICMP-Echo-Request-Probe wird aufgebaut und mit einer Prüfsumme versehen.
4. Die Probe wird mit `sendto()` an das Ziel gesendet.
5. Mit `select()` wartet das Programm blockierend auf eine Antwort.
6. Die Antwort wird auf passende Identifier und Sequenznummer geprüft.
7. Die Adresse des antwortenden Routers und die RTT werden ausgegeben.
8. Danach wird der TTL erhöht und der nächste Hop getestet.
9. Bei einem Echo Reply oder einer passenden Destination-Unreachable-Meldung
   wird die Route als am Ziel angekommen betrachtet.

Wenn innerhalb des Timeouts keine passende Antwort eintrifft, zeigt die
Ausgabe für diesen Probe `*`. Das kann bedeuten, dass ein Router ICMP filtert,
die Antwort verloren ging oder das Netzwerk die Probes nicht weiterleitet.

## Project objectives

- Understand IPv4 routing and the TTL field.
- Create and use network sockets in C.
- Handle ICMP responses such as `Time Exceeded` and `Destination Unreachable`.
- Resolve a destination IPv4 address or hostname.
- Reproduce the output style of the system `traceroute` command.
- Handle network errors without crashes or undefined behavior.

## Mandatory requirements

The mandatory implementation must:

- Produce an executable named `ft_traceroute`.
- Be written in C.
- Include a Makefile with the usual targets.
- Accept one simple IPv4 address, hostname, or FQDN as the destination.
- Support the `--help` option.
- Display the route without performing reverse DNS lookups for every hop.
- Use blocking sockets; sockets must not be configured with `O_NONBLOCK`.
- Use only the C standard library and permitted libc functions for the
  mandatory part.
- Never call the system `traceroute` command or reuse its source code.
- Match the system `traceroute` indentation and output style closely.
- Handle errors reasonably without unexpected termination.

The expected command-line form is:

```text
./ft_traceroute [--help] <IPv4 address | hostname | FQDN>
```

## Flags

### Mandatory flag

| Flag | Description |
| --- | --- |
| `--help` | Display usage information and exit. |

The mandatory version intentionally accepts no other flags. The destination is
provided as the only positional argument.

### Bonus flags

The following bonus flags are implemented:

| Flag | Description |
| --- | --- |
| `-i interface` | Bind probes to a network interface. |
| `-m hops` | Set the maximum hop limit. |
| `-p port` | Set the probe identifier port value. |
| `-s size` | Set the ICMP payload size. |
| `-q count` | Set the number of probes per hop. |
| `-N count` | Set the maximum simultaneous-probe setting. |
| `-t tos` | Set the IPv4 Type of Service value. |
| `-l label` | Set the flow-label setting. |
| `-V` | Not a valid bonus flag for this project. |

`-N` is accepted and validated; probes are currently sent sequentially to
preserve simple blocking-socket behavior. IPv4 has no native flow-label field,
so `-l` is retained as a validated configuration value.

The `-V` option is not a valid bonus requirement. Bonus work is evaluated only
when the mandatory part is complete and works correctly.

## Requirements and permissions

- Linux kernel version 4.0 or newer.
- A C compiler such as `cc` or `gcc`.
- GNU Make.
- Permission to create and use the sockets required for traceroute.

Raw-socket operations commonly require root privileges or the appropriate
Linux capability:

```sh
sudo setcap cap_net_raw+ep ./ft_traceroute
```

Only grant capabilities to a binary you trust.

## Build

Once the source files and Makefile are present, build the project from this
directory:

```sh
make
```

Usual Makefile targets:

```sh
make        # Build ft_traceroute
make clean  # Remove object files
make fclean # Remove object files and the executable
make re     # Rebuild from scratch
```

## Usage examples

Display the help message:

```sh
./ft_traceroute --help
```

Trace a local destination:

```sh
sudo ./ft_traceroute 127.0.0.1
```

Trace a public IPv4 address:

```sh
sudo ./ft_traceroute 8.8.8.8
```

Trace a hostname or FQDN:

```sh
sudo ./ft_traceroute example.com
```

The exact number of hops and responses depends on the network, firewall
configuration, routing, and destination. A `*` generally means that a probe
did not receive a response before its timeout.

## Tests and verification

Build the project and verify the mandatory interface:

```sh
make re
./ft_traceroute --help
```

Test invalid invocations. These commands should print an error and exit
without attempting a trace:

```sh
./ft_traceroute
./ft_traceroute --unknown
./ft_traceroute 127.0.0.1 extra-argument
```

Test a local route:

```sh
sudo ./ft_traceroute 127.0.0.1
```

Test hostname and FQDN resolution:

```sh
sudo ./ft_traceroute google.com
sudo ./ft_traceroute wikipedia.org
sudo ./ft_traceroute -m 5 -q 1 google.com
sudo ./ft_traceroute -s 64 -t 16 google.com
sudo ./ft_traceroute -p 33435 -N 2 google.com
sudo ./ft_traceroute -l 12345 google.com
sudo ./ft_traceroute -i eth0 google.com
```

Test a numeric IPv4 destination:

```sh
sudo ./ft_traceroute 8.8.8.8
```

Expected output starts with a destination header followed by up to 30 hops:

```text
traceroute to google.com (142.250.17.14), 30 hops max
 1  192.168.1.1  1.234 ms  1.105 ms  1.087 ms
 2  * * *
```

If every hop is shown as `* * *`, DNS resolution and execution can still be
working correctly; the network may block ICMP traceroute responses.

Test bonus argument validation without a live trace:

```sh
./ft_traceroute -m 0 google.com       # invalid maximum hop count
./ft_traceroute -q 0 google.com       # invalid probe count
./ft_traceroute -s 70000 google.com   # invalid packet size
./ft_traceroute -t 300 google.com     # invalid TOS value
```

For comparison, install the reference command on Debian or Ubuntu:

```sh
sudo apt update
sudo apt install traceroute man-db manpages
man traceroute
traceroute 127.0.0.1
```

If `apt` cannot find `traceroute`, check that the distribution repositories
are enabled and run `sudo apt update` again. The reference command is for
comparison only; `ft_traceroute` must implement the behavior itself.

## Expected output

The output should identify each hop in order, for example:

```text
traceroute to example.com (93.184.216.34), 30 hops max
 1  192.168.1.1  1.234 ms  1.105 ms  1.087 ms
 2  10.0.0.1     8.421 ms  8.390 ms  8.512 ms
 3  * * *
```

The exact addresses, hop count, and timings vary by network.

## Project status

The mandatory implementation is present:

- `--help` argument handling.
- IPv4 address, hostname, and FQDN resolution with `getaddrinfo()`.
- Blocking raw ICMP socket operation.
- TTL values from 1 through 30.
- Three probes per hop with one-second response timeouts.
- Numeric hop output without reverse DNS lookups.
- ICMP Time Exceeded, Echo Reply, and Destination Unreachable handling.
- `SIGINT` handling and error reporting.

The listed bonus flags are implemented with the behavior described above.

## Reference

- `en.subject.pdf` — official project subject, version 5.0.
