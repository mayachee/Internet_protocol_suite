# ft_ping

`ft_ping` is a small IPv4 ping implementation written in C. It resolves a
hostname or IPv4 address, sends ICMP Echo Requests through a raw socket, waits
for matching Echo Replies, and prints per-packet timing and final packet-loss
statistics.

The project is intended for Linux environments and focuses on the core
behavior of `ping`: ICMP packet construction, checksum calculation, raw socket
I/O, reply matching, timeouts, and signal handling.

## Requirements

- A C compiler
- GNU Make
- Linux or another POSIX-like environment providing IPv4 raw ICMP sockets
- Permission to create raw sockets

On Linux, run the program as `root`, or grant the executable the
`CAP_NET_RAW` capability:

```sh
sudo setcap cap_net_raw+ep ./ft_ping
```

Only grant this capability to a binary you trust.

## Build

From this directory:

```sh
make
```

The executable is created as `./ft_ping`. The Makefile also provides:

```sh
make clean   # Remove object files
make fclean  # Remove object files and the executable
make re      # Rebuild from scratch
```

## Usage

```text
./ft_ping [options] <host>
```

Examples:

```sh
sudo ./ft_ping 127.0.0.1
sudo ./ft_ping example.com
sudo ./ft_ping -v 192.0.2.1
./ft_ping -?
```

The program continues sending requests until interrupted with `Ctrl-C`, or
until `-w` sets a deadline.

## Flags

### Mandatory flags

These flags are part of the required `ft_ping` interface:

| Flag | Description | Status |
| --- | --- | --- |
| `-v` | Display relevant ICMP error messages and continue running. | Implemented |
| `-?` | Display the help message and exit. | Implemented |

### Bonus flags

These optional extensions are implemented:

| Flag | Description |
| --- | --- |
| `-f` | Flood mode |
| `-l count` | Preload the first `count` requests |
| `-n` | Numeric output |
| `-w deadline` | Stop after the specified number of seconds |
| `-W timeout` | Set the per-request timeout in seconds |
| `-p pattern` | Fill the payload with a repeating hexadecimal pattern |
| `-r` | Set `SO_DONTROUTE` and bypass routing tables |
| `-s size` | Set the ICMP payload size in bytes |
| `-T ttl` / `--ttl ttl` | Set the IPv4 time-to-live from 1 to 255 |
| `--ip-timestamp` | Request an IPv4 timestamp option |

For `-p`, use an even-length hexadecimal string of up to 16 bytes, for example
`-p deadbeef`.

If no host is supplied, more than one host is supplied, or an unknown option is
used, the program prints an error and exits.

## Verification examples

Run these commands from the `ft_ping` directory. Most tests need root
privileges because the program creates a raw ICMP socket:

```sh
make
sudo ./ft_ping -?                         # Display help
sudo ./ft_ping 127.0.0.1                  # Basic local ping
sudo ./ft_ping -n -w 3 127.0.0.1         # Numeric output for 3 seconds
sudo ./ft_ping -s 128 -W 0.5 -w 3 127.0.0.1
                                          # Custom payload and timeout
sudo ./ft_ping -p deadbeef -w 3 127.0.0.1 # Repeating hexadecimal pattern
sudo ./ft_ping -T 32 -w 3 127.0.0.1      # Custom IPv4 TTL
sudo ./ft_ping -l 3 -w 3 127.0.0.1        # Preload three requests
sudo ./ft_ping -f -w 3 127.0.0.1         # Flood mode
sudo ./ft_ping -v -w 3 192.0.2.1         # Verbose errors/timeouts
```

Use `./ft_ping`, not `ft_ping`, when running a program from the current
directory. The latter only works if the current directory is in your `PATH`:

```sh
sudo ./ft_ping -n -w 3 192.0.2.1
```

For a normal successful test, expect `icmp_seq`, `ttl`, and `time` on each
reply, followed by packet-loss and RTT statistics. `192.0.2.1` is reserved for
documentation and testing, so it is useful for observing timeout behavior
without relying on a real public host.

The argument parser can be tested without opening a socket:

```sh
./ft_ping -s invalid 127.0.0.1    # Expect: invalid value for -s
./ft_ping -p abc 127.0.0.1        # Expect: invalid value for -p
./ft_ping --unknown 127.0.0.1     # Expect: unknown option
./ft_ping                       # Expect: usage information
```

If raw sockets are unavailable, the program will report
`ft_ping: socket: Operation not permitted`. Grant the executable the required
capability instead of using `sudo` if preferred:

```sh
sudo setcap cap_net_raw+ep ./ft_ping
./ft_ping -w 3 127.0.0.1
```

## Output

For each matching reply, `ft_ping` reports the number of bytes received, the
source address, ICMP sequence number, IPv4 TTL, and round-trip time:

```text
PING example.com (203.0.113.10): 56 data bytes
64 bytes from 203.0.113.10: icmp_seq=0 ttl=52 time=18.421 ms
```

When a reply is not received within the configured timeout, it waits for the
next request without printing a per-packet timeout line. At the end, it prints
a statistics header followed by the number of transmitted, received, and lost
packets.

## Project structure

```text
.
├── includes/
│   └── ft_ping.h       Public declarations and shared constants
├── src/
│   ├── main.c          Program entry point
│   ├── parse.c         Command-line parsing and help output
│   ├── resolve.c       Hostname and IPv4 address resolution
│   ├── packet.c        ICMP packet creation and checksum calculation
│   ├── loop.c          Raw socket loop, timing, timeouts, and signals
│   ├── reply.c         Reply formatting helper
│   └── stats.c         Packet statistics
├── Makefile
└── en.subject.pdf      Project subject
```

## Notes and limitations

- The implementation currently targets IPv4 (`AF_INET`) only.
- The default ICMP payload is 56 bytes; `-s` can change it.
- `-n` is accepted for compatibility; replies are always printed numerically
  because reverse DNS is not performed.
- Raw socket access may be restricted by the operating system, container, or
  network namespace.
- Network behavior depends on the destination and local firewall rules.

https://www.geeksforgeeks.org/linux-unix/ping-command-in-linux-with-examples/