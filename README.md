# Internetprotokollfamilie

<img width="1266" height="598" alt="image" src="https://github.com/user-attachments/assets/04f5d4cd-8094-448c-aa91-4e5284afd32d" />

### Layer 1: Link layer (the local link)

The link is every machine you can reach without crossing a router: your Wi-Fi network, or one Ethernet segment.
It frames the packet, hands it to the network card, and sends it over the physical medium: cable, radio or fiber.
Hosts on a link are identified by MAC addresses (media access control), burned into the network card. The suite includes a way to translate IP addresses into MAC addresses; that is ARP (Address Resolution Protocol). I’m adding that detail myself.
The model is hardware independent. It also runs over virtual links such as VPNs and tunnels.
The physical layer below it (voltages, radio signals) is assumed to exist but not defined. Some textbooks split this layer in two to make a five-layer model.

### Layer 2: Internet layer (between networks)

This layer is what creates “the Internet”. IP gives every host an address, and routers forward packets hop by hop toward the destination.

IPv4 uses 32-bit addresses (about 4 billion hosts). IPv6, standardized in 1998, uses 128-bit addresses to solve the exhaustion problem.
IP is best effort and connectionless. A packet can be lost, duplicated, delayed or reordered, and IP does not care.
IP carries many upper protocols, each marked by a protocol number in the IP header. ICMP is protocol 1 and IGMP is protocol 2. This is where your ft_ping lives: ICMP rides directly on IP, with no transport layer in between.
Historically a router was called a gateway. Every router has an interface on each network it connects.

### Layer 3: Transport layer (host to host)

IP delivers packets to a machine, but a machine runs many programs. The transport layer solves that with ports, numbered channels, one per communication.
UDP is connectionless and unreliable. It uses a checksum for error detection only. It suits streaming, voice and short query/response exchanges like DNS, where setting up a connection costs more than the message.
TCP is connection-oriented and provides a reliable byte stream. Data arrives in order, errors are minimized, duplicates are discarded, lost packets are resent, and congestion is controlled.
SCTP is reliable like TCP but message-oriented, with several streams in one connection and multihoming (one connection, several IP addresses).
QUIC is a newer transport carried inside UDP packets, and HTTP/3 works only over it.

### Layer 4: Application layer (process to process)

Here live the protocols users actually touch: HTTP, FTP, SMTP, SSH, DHCP.
Servers listen on well-known ports reserved by IANA: HTTP uses 80 and Telnet uses 23. Clients use temporary ephemeral ports for the duration of the exchange.
The page distinguishes user protocols (FTP) from support protocols (DNS), which serve the network infrastructure.
Routing protocols such as BGP, OSPF and RIP also count as application layer here, because they travel inside IP.
The OSI model’s session and presentation layers are folded into this layer. Their jobs are done by libraries, APIs and protocols like MIME.

# Netzwerkprotokolle – TCP/IP-Protokollstapel

> Eine strukturierte Übersicht wichtiger Netzwerkprotokolle und ihrer Zuordnung zum TCP/IP- und OSI-Modell.

## Inhaltsverzeichnis

- [Überblick](#überblick)
- [Anwendungsschicht](#anwendungsschicht)
- [Transportschicht](#transportschicht)
- [Internetschicht](#internetschicht)
- [Netzzugangsschicht](#netzzugangsschicht)
- [Literatur](#literatur)
- [Weiterführende Links](#weiterführende-links)

## Überblick

| TCP/IP-Schicht | Ungefähre OSI-Zuordnung | Aufgabe |
| --- | --- | --- |
| Anwendungsschicht | 5–7 | Dienste für Anwendungen und Kommunikation |
| Transportschicht | 4 | Ende-zu-Ende-Transport von Daten |
| Internetschicht | 3 | Adressierung und Weiterleitung von Paketen |
| Netzzugangsschicht | 1–2 | Übertragung über lokale Netzwerkmedien |

> **Hinweis:** Die Zuordnung ist vereinfacht. Einige Protokolle (z. B. TLS, IPsec und L2TP) lassen sich nicht eindeutig einer einzigen Schicht zuordnen.

## Anwendungsschicht

| Protokoll | Bedeutung / Beschreibung |
| --- | --- |
| [DNS](https://de.wikipedia.org/wiki/Domain_Name_System "Domain Name System") | (Domain Name System) – Auflösung von Domainnamen zu IP-Adressen |
| [DoIP](https://de.wikipedia.org/wiki/DoIP "DoIP") | (Diagnostics over IP) – Transportprotokoll für Fahrzeugdiagnose |
| [FTP](https://de.wikipedia.org/wiki/File_Transfer_Protocol "File Transfer Protocol") | (File Transfer Protocol) – Dateitransfer |
| [FTPS](https://de.wikipedia.org/wiki/FTP_%C3%BCber_SSL "FTP über SSL") | (FTP über [SSL/TLS](https://de.wikipedia.org/wiki/Transport_Layer_Security "Transport Layer Security")) – Erweiterung von FTP für verschlüsselten Dateitransfer |
| [HTTP](https://de.wikipedia.org/wiki/Hypertext_Transfer_Protocol "Hypertext Transfer Protocol") | (Hypertext Transfer Protocol) |
| [HTTPS](https://de.wikipedia.org/wiki/Hypertext_Transfer_Protocol_Secure "Hypertext Transfer Protocol Secure") | (Hypertext Transfer Protocol Secure) |
| [WS](https://de.wikipedia.org/wiki/WebSocket "WebSocket") | (WebSocket Protocol) |
| [IMAP](https://de.wikipedia.org/wiki/Internet_Message_Access_Protocol "Internet Message Access Protocol") | (Internet Message Access Protocol) – Zugriff auf E-Mails |
| [IPFIX](https://de.wikipedia.org/wiki/Internet_Protocol_Flow_Information_Export "Internet Protocol Flow Information Export") | (Internet Protocol Flow Information Export) |
| [LLMNR](https://de.wikipedia.org/wiki/Link-local_Multicast_Name_Resolution "Link-local Multicast Name Resolution") | (Link-local Multicast Name Resolution) |
| [NDMP](https://de.wikipedia.org/wiki/Network_Data_Management_Protocol "Network Data Management Protocol") | (Network Data Management Protocol, ndmp.org, kein IETF RFC) |
| [MBS/IP](https://de.wikipedia.org/wiki/Multi-purpose_Business_Security_over_IP "Multi-purpose Business Security over IP") | (Multi-purpose Business Security over IP) |
| [NNTP](https://de.wikipedia.org/wiki/Network_News_Transfer_Protocol "Network News Transfer Protocol") | (Network News Transfer Protocol) – Diskussionsforen ([Usenet](https://de.wikipedia.org/wiki/Usenet "Usenet")) |
| [NTP](https://de.wikipedia.org/wiki/Network_Time_Protocol "Network Time Protocol") | (Network Time Protocol) |
| [POP3](https://de.wikipedia.org/wiki/Post_Office_Protocol "Post Office Protocol") | (Post Office Protocol, Version 3) – E-Mail-Abruf |
| [PTP](https://de.wikipedia.org/wiki/Precision_Time_Protocol "Precision Time Protocol") | (Precision Time Protocol) – Zeitsynchronisation von Uhren in einem Netzwerk |
| [RDP](https://de.wikipedia.org/wiki/Remote_Desktop_Protocol "Remote Desktop Protocol") | (Remote Desktop Protocol) – Darstellen und Steuern von Desktops auf fernen Computern ([Microsoft](https://de.wikipedia.org/wiki/Microsoft "Microsoft")) |
| [RTP](https://de.wikipedia.org/wiki/Real-Time_Transport_Protocol "Real-Time Transport Protocol") | (Real-Time Transport Protocol) |
| [SIP](https://de.wikipedia.org/wiki/Session_Initiation_Protocol "Session Initiation Protocol") | (Session Initiation Protocol) – Aufbau, Steuerung und Abbau von Kommunikationssitzung ([VoIP](https://de.wikipedia.org/wiki/IP-Telefonie "IP-Telefonie")) |
| [SNMP](https://de.wikipedia.org/wiki/Simple_Network_Management_Protocol "Simple Network Management Protocol") | (Simple Network Management Protocol) – Verwaltung von Geräten im Netzwerk |
| [SMTP](https://de.wikipedia.org/wiki/Simple_Mail_Transfer_Protocol "Simple Mail Transfer Protocol") | (Simple Mail Transfer Protocol) – E-Mail-Versand |
| [SOCKS](https://de.wikipedia.org/wiki/SOCKS "SOCKS") | (Internet [Sockets](https://de.wikipedia.org/wiki/Socket "Socket")-Protokoll) |
| [SSH](https://de.wikipedia.org/wiki/Secure_Shell "Secure Shell") | (Secure Shell) – verschlüsseltes *remote terminal* |
| [SCP](https://de.wikipedia.org/wiki/Secure_Copy "Secure Copy") | (Secure Copy) – Dateitransfer (verwendet SSH-Session) |
| [SFTP](https://de.wikipedia.org/wiki/SSH_File_Transfer_Protocol "SSH File Transfer Protocol") | – SSH Dateitransfer (verwendet SSH-Session) |
| [Telnet](https://de.wikipedia.org/wiki/Telnet "Telnet") | – Unverschlüsseltes [Login](https://de.wikipedia.org/wiki/Login_\(Informationstechnik\) "Login (Informationstechnik)") auf entfernten Rechnern (*remote terminal*) |
| [XMPP](https://de.wikipedia.org/wiki/Extensible_Messaging_and_Presence_Protocol "Extensible Messaging and Presence Protocol") | (Extensible Message and Presence Protocol) |
| [Z39.50](https://de.wikipedia.org/wiki/Z39.50 "Z39.50") | – Abfrage von Informationssystemen |

## Transportschicht

| Protokoll | Bedeutung / Beschreibung |
| --- | --- |
| [TCP](https://de.wikipedia.org/wiki/Transmission_Control_Protocol "Transmission Control Protocol") | (Transmission Control Protocol) – Übertragung von Datenströmen ([verbindungsorientiert](https://de.wikipedia.org/wiki/Nachrichtenverbindung "Nachrichtenverbindung"), zuverlässig) |
| [UDP](https://de.wikipedia.org/wiki/User_Datagram_Protocol "User Datagram Protocol") | (User Datagram Protocol) – Übertragung von Datenpaketen ([verbindungslos](https://de.wikipedia.org/wiki/Nachrichtenverbindung "Nachrichtenverbindung"), unzuverlässig, geringer Overhead) |
| [SCTP](https://de.wikipedia.org/wiki/Stream_Control_Transmission_Protocol "Stream Control Transmission Protocol") | (Stream Control Transmission Protocol) – Transportprotokoll |
| [TLS](https://de.wikipedia.org/wiki/Transport_Layer_Security "Transport Layer Security") | (Transport Layer Security, ehemals 'Secure Socket Layer [SSL]) – Erweiterung von TCP um Verschlüsselung |
| [DTLS](https://de.wikipedia.org/wiki/Datagram_Transport_Layer_Security "Datagram Transport Layer Security") | (Datagram Transport Layer Security) – Auf TLS basierendes Verschlüsselungsprotokoll, welches auch über zustandslose Protokolle wie UDP übertragen werden kann |

## Internetschicht

| Protokoll | Bedeutung / Beschreibung |
| --- | --- |
| [IP](https://de.wikipedia.org/wiki/Internet_Protocol "Internet Protocol") | (Internet Protocol) – Datenpaket-Übertragung (verbindungslos) |
| [IPsec](https://de.wikipedia.org/wiki/IPsec "IPsec") | (Internet Protocol Security) – Sichere Datenpaket-Übertragung (verbindungslos) |
| [ICMP](https://de.wikipedia.org/wiki/Internet_Control_Message_Protocol "Internet Control Message Protocol") | (Internet Control Message Protocol) – Kontrollnachrichten (zum Beispiel Fehlermeldungen), Teil jeder IP-Implementierung |
| [IGRP](https://de.wikipedia.org/wiki/Interior_Gateway_Routing_Protocol "Interior Gateway Routing Protocol") | (Interior Gateway Routing Protocol) – Informationsaustausch zwischen Routern ([Distanzvektor](https://de.wikipedia.org/wiki/Distanzvektoralgorithmus "Distanzvektoralgorithmus")) (veraltet – wird ersetzt durch [EIGRP](https://de.wikipedia.org/wiki/Enhanced_Interior_Gateway_Routing_Protocol "Enhanced Interior Gateway Routing Protocol")) |
| [EIGRP](https://de.wikipedia.org/wiki/Enhanced_Interior_Gateway_Routing_Protocol "Enhanced Interior Gateway Routing Protocol") | (Enhanced Interior Gateway Routing Protocol) – Informationsaustausch zwischen Routern via IP |
| [OSPF](https://de.wikipedia.org/wiki/Open_Shortest_Path_First "Open Shortest Path First") | (Open Shortest Path First) – Informationsaustausch zwischen Routern (Linkzustand) via [IP](https://de.wikipedia.org/wiki/Internet_Protocol "Internet Protocol") |
| [BGP](https://de.wikipedia.org/wiki/Border_Gateway_Protocol "Border Gateway Protocol") | (Border Gateway Protocol) – Informationsaustausch zwischen autonomen Systemen im Internet ([Pfadvektor](https://de.wikipedia.org/wiki/Pfadvektorprotokoll "Pfadvektorprotokoll")) via [TCP](https://de.wikipedia.org/wiki/Transmission_Control_Protocol "Transmission Control Protocol") |
| [RIP](https://de.wikipedia.org/wiki/Routing_Information_Protocol "Routing Information Protocol") | (Routing Information Protocol) – Informationsaustausch zwischen Routern ([Distanzvektor](https://de.wikipedia.org/wiki/Distanzvektoralgorithmus "Distanzvektoralgorithmus")) via [UDP](https://de.wikipedia.org/wiki/User_Datagram_Protocol "User Datagram Protocol") |
| [IGMP](https://de.wikipedia.org/wiki/Internet_Group_Management_Protocol "Internet Group Management Protocol") | (Internet Group Management) – Organisation von [Multicast](https://de.wikipedia.org/wiki/Multicast "Multicast")-Gruppen, Bestandteil von IP auf allen [Hosts](https://de.wikipedia.org/wiki/Hostrechner "Hostrechner"), die den Empfang von IP-Multicasts unterstützen |

## Netzzugangsschicht

| Protokoll | Bedeutung / Beschreibung |
| --- | --- |
| [Ethernet](https://de.wikipedia.org/wiki/Ethernet "Ethernet") | – Netzwerkstandard IEEE 802.3 |
| [WLAN](https://de.wikipedia.org/wiki/IEEE_802.11 "IEEE 802.11") | – Netzwerkstandard IEEE 802.11 |
| [PPP](https://de.wikipedia.org/wiki/Point-to-Point_Protocol "Point-to-Point Protocol") | – Point-to-Point Protokoll, RFC 1661[\[2\]](https://de.wikipedia.org/wiki/Internetprotokollfamilie#cite_note-2) |
| [Token Bus](https://de.wikipedia.org/wiki/Token_Bus "Token Bus") | – Netzwerkstandard IEEE 802.4 |
| [Token Ring](https://de.wikipedia.org/wiki/Token_Ring "Token Ring") | – Netzwerkstandard IEEE 802.5 |
| [FDDI](https://de.wikipedia.org/wiki/Fiber_Distributed_Data_Interface "Fiber Distributed Data Interface") | – Fiber Distributed Data Interface |
| [L2TP](https://de.wikipedia.org/wiki/Layer_2_Tunneling_Protocol "Layer 2 Tunneling Protocol") | (Layer 2 Tunneling Protocol) |
| [RARP](https://de.wikipedia.org/wiki/Reverse_Address_Resolution_Protocol "Reverse Address Resolution Protocol") | (Reverse Address Resolution Protocol) – Adressumsetzung zwischen Geräte- (MAC) und IP-Adressen (veraltet – wird ersetzt durch [BOOTP](https://de.wikipedia.org/wiki/Bootstrap_Protocol "Bootstrap Protocol")) |
| [IPoAC](https://de.wikipedia.org/wiki/Internet_Protocol_over_Avian_Carriers "Internet Protocol over Avian Carriers") | (Internet Protocol over Avian Carriers) – Internet-Protokoll mittels fliegender Träger – RFC 1149[\[3\]](https://de.wikipedia.org/wiki/Internetprotokollfamilie#cite_note-3) |
| [William Richard Stevens](https://de.wikipedia.org/wiki/Richard_Stevens "Richard Stevens") | : *TCP/IP.* [ISBN 3-8266-5042-5](https://de.wikipedia.org/wiki/Special:BookSources/3826650425) (Standardwerk). |
| [Andrew S. Tanenbaum](https://de.wikipedia.org/wiki/Andrew_S._Tanenbaum "Andrew S. Tanenbaum") | : *Computernetzwerke.* 5., aktualisierte Auflage, Pearson Studium, München 2012, [ISBN 978-3-86894-137-1](https://de.wikipedia.org/wiki/Special:BookSources/9783868941371) (Standardwerk). |
| [James F. Kurose](https://de.wikipedia.org/wiki/James_F._Kurose "James F. Kurose") | , Keith W. Ross: *Computernetzwerke.* [ISBN 978-3-8273-7330-4](https://de.wikipedia.org/wiki/Special:BookSources/9783827373304) (Standardwerk). |
| [Netzwerktechnologie TCP/IP](https://www.highgames.com/?set=hardwareview\&view=12) | — |

## Literatur

- [William Richard Stevens](https://de.wikipedia.org/wiki/Richard_Stevens "Richard Stevens"): *TCP/IP.* [ISBN 3-8266-5042-5](https://de.wikipedia.org/wiki/Special:BookSources/3826650425) (Standardwerk).
- [Andrew S. Tanenbaum](https://de.wikipedia.org/wiki/Andrew_S._Tanenbaum "Andrew S. Tanenbaum"): *Computernetzwerke.* 5., aktualisierte Auflage, Pearson Studium, München 2012, [ISBN 978-3-86894-137-1](https://de.wikipedia.org/wiki/Special:BookSources/9783868941371) (Standardwerk).
- [James F. Kurose](https://de.wikipedia.org/wiki/James_F._Kurose "James F. Kurose"), Keith W. Ross: *Computernetzwerke.* [ISBN 978-3-8273-7330-4](https://de.wikipedia.org/wiki/Special:BookSources/9783827373304) (Standardwerk).
- Manfred Weiss: *TCP/IP Handbuch.* [ISBN 3-7723-5026-7](https://de.wikipedia.org/wiki/Special:BookSources/3772350267)
- Uyless Black: *TCP/IP & Related Protocols.* [ISBN 0-07-005560-2](https://de.wikipedia.org/wiki/Special:BookSources/0070055602).
- Craig Hunt: *TCP/IP.* [ISBN 3-89721-110-6](https://de.wikipedia.org/wiki/Special:BookSources/3897211106).

---

*Zusammengestellt als Lernübersicht für Netzwerktechnik und Systemintegration.*
