# La BoxEPP

## Integration hostapd et dnsmasq

je peux vous guider à travers la configuration de hostapd et dnsmasq pour créer un hotspot Wi-Fi sur un Raspberry Pi. Voici les étapes de base :  

1. Installation des logiciels nécessaires :  
Assurez-vous d'avoir hostapd et dnsmasq installés sur votre Raspberry Pi. Si ce n'est pas déjà le cas, vous pouvez les installer en utilisant les commandes suivantes :  

```bash
sudo apt-get update
sudo apt-get install hostapd dnsmasq
```
2. Configuration de hostapd :  
Créez ou modifiez le fichier de configuration de hostapd pour définir les paramètres du point d'accès Wi-Fi. Voici un exemple de configuration pour un point d'accès nommé "MonHotspot" avec le mot de passe "MotDePasse123" :  

Créez le fichier /etc/hostapd/hostapd.conf et ajoutez-y les lignes suivantes :  

```makefile
interface=wlan0
driver=nl80211
ssid=MonHotspot
hw_mode=g
channel=7
wmm_enabled=0
macaddr_acl=0
auth_algs=1
ignore_broadcast_ssid=0
wpa=2
wpa_passphrase=MotDePasse123
wpa_key_mgmt=WPA-PSK
wpa_pairwise=TKIP
rsn_pairwise=CCMP
```

3. Configuration de dnsmasq :  
Configurez dnsmasq pour gérer les adresses IP des appareils connectés au hotspot. Éditez le fichier /etc/dnsmasq.conf et ajoutez-y les lignes suivantes à la fin du fichier :

```go
interface=wlan0
dhcp-range=192.168.4.2,192.168.4.20,255.255.255.0,24h
```

4. Désactiver le service DHCP pour wlan0 :  
Pour éviter les conflits avec dnsmasq, désactivez le service DHCP pour l'interface wlan0. Éditez le fichier `/etc/dhcpcd.conf` et ajoutez `denyinterfaces wlan0` à la fin du fichier.  

5. Démarrage automatique des services :  
Pour que hostapd et dnsmasq démarrent automatiquement au démarrage, exécutez les commandes suivantes :

```bash
sudo systemctl unmask hostapd
sudo systemctl enable hostapd
sudo systemctl enable dnsmasq
```

6. Redémarrage :  
Redémarrez votre Raspberry Pi pour appliquer les modifications :

```bash
sudo reboot
```
Une fois votre Raspberry Pi redémarré, il devrait diffuser un réseau Wi-Fi avec le SSID que vous avez défini dans hostapd.conf. Les appareils pourront se connecter à ce hotspot en utilisant le mot de passe que vous avez spécifié. Assurez-vous que l'interface Wi-Fi de votre Raspberry Pi est bien configurée comme wlan0 dans les fichiers de configuration.  

N'oubliez pas de tester la connexion au hotspot avec un appareil Wi-Fi pour vous assurer que tout fonctionne correctement.
