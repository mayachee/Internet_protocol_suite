# Internetprotokollfamilie

<img width="1266" height="598" alt="image" src="https://github.com/user-attachments/assets/04f5d4cd-8094-448c-aa91-4e5284afd32d" />

Layer 1: Link layer (the local link)
The link is every machine you can reach without crossing a router: your Wi-Fi network, or one Ethernet segment.
It frames the packet, hands it to the network card, and sends it over the physical medium: cable, radio or fiber.
Hosts on a link are identified by MAC addresses (media access control), burned into the network card. The suite includes a way to translate IP addresses into MAC addresses; that is ARP (Address Resolution Protocol). I’m adding that detail myself.
The model is hardware independent. It also runs over virtual links such as VPNs and tunnels.
The physical layer below it (voltages, radio signals) is assumed to exist but not defined. Some textbooks split this layer in two to make a five-layer model.
Layer 2: Internet layer (between networks)

This layer is what creates “the Internet”. IP gives every host an address, and routers forward packets hop by hop toward the destination.

IPv4 uses 32-bit addresses (about 4 billion hosts). IPv6, standardized in 1998, uses 128-bit addresses to solve the exhaustion problem.
IP is best effort and connectionless. A packet can be lost, duplicated, delayed or reordered, and IP does not care.
IP carries many upper protocols, each marked by a protocol number in the IP header. ICMP is protocol 1 and IGMP is protocol 2. This is where your ft_ping lives: ICMP rides directly on IP, with no transport layer in between.
Historically a router was called a gateway. Every router has an interface on each network it connects.
Layer 3: Transport layer (host to host)

IP delivers packets to a machine, but a machine runs many programs. The transport layer solves that with ports, numbered channels, one per communication.

UDP is connectionless and unreliable. It uses a checksum for error detection only. It suits streaming, voice and short query/response exchanges like DNS, where setting up a connection costs more than the message.
TCP is connection-oriented and provides a reliable byte stream. Data arrives in order, errors are minimized, duplicates are discarded, lost packets are resent, and congestion is controlled.
SCTP is reliable like TCP but message-oriented, with several streams in one connection and multihoming (one connection, several IP addresses).
QUIC is a newer transport carried inside UDP packets, and HTTP/3 works only over it.
Layer 4: Application layer (process to process)
Here live the protocols users actually touch: HTTP, FTP, SMTP, SSH, DHCP.
Servers listen on well-known ports reserved by IANA: HTTP uses 80 and Telnet uses 23. Clients use temporary ephemeral ports for the duration of the exchange.
The page distinguishes user protocols (FTP) from support protocols (DNS), which serve the network infrastructure.
Routing protocols such as BGP, OSPF and RIP also count as application layer here, because they travel inside IP.
The OSI model’s session and presentation layers are folded into this layer. Their jobs are done by libraries, APIs and protocols like MIME.
