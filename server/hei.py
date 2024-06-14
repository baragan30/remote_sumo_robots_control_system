from scapy.all import ARP, Ether, srp

def arp_scan(ip_range):
    arp = ARP(pdst=ip_range)
    ether = Ether(dst="ff:ff:ff:ff:ff:ff")
    packet = ether / arp
    result = srp(packet, timeout=3, verbose=0)[0]
    devices = []
    for sent, received in result:
        devices.append({'ip': received.psrc, 'mac': received.hwsrc})
    return devices

if __name__ == "__main__":
    ip_range = "192.168.1.0/24"  # Change to your local network range
    devices = arp_scan(ip_range)
    print("IP addresses of devices in the network:")
    for device in devices:
        print(f"IP: {device['ip']}, MAC: {device['mac']}")