#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>


/*  Esta unión divide las direcciones de 32 bits en 4 bytes. 
    Es muy importante usar no signados de lo contrario, 
    no podríamos almacenar valores de hasta 255.
 */
typedef union {
	uint32_t val;
	unsigned char byte[4];
} uIp32;



/* Esta estructura es utilizada por calcNet () */
typedef struct ipcalcsubnet{
	unsigned int hosts; /* max. posible hosts */
	struct in_addr netmask;		/* máscara de red */
	struct in_addr netid;   	/* id de la red */
	struct in_addr hostmin;		/* primera ip posible de esta red */
	struct in_addr hostmax;		/* ultima ip posible de esta red*/
	struct in_addr broadcast;	/* broadcast de la red */
	struct in_addr wildcard;	/* wildcard */
} ipnet;


/*
    Esta función calcula lo que necesitamos de la red que
    necesita la estructura ipcalcsubnet. Se tiene que llenar
    esta estructura primero con un netid y una máscara de subred
 */  
int calcNet(ipnet* netdata)
{
	/*
	    Digamos que completamos nuestro netid con la dirección 192.168.10.3
        y una máscara de subred 255.255.255.0. entonces 192.168.10.3 no es el verdadero id de red.
        es solo una ip de la red 192.168.10.0/24.
        Podemos calcular fácilmente el netid:
        haciendo una operación AND sobre los bits en la máscara de red e ip.
	 */  
	netdata->netid.s_addr = netdata->netid.s_addr & netdata->netmask.s_addr;
	/*
	    la wildcard es la inversa de nuestra máscara de subred.
        entonces, si nuestra máscara de subred es 255.255.255.0, nuestro comodín sería 0.0.0.255
        haciendo una operación NOT en la máscara de red
	 */  
	netdata->wildcard.s_addr = ~netdata->netmask.s_addr;
	/*
	    La dirección de difusión es la dirección IP más alta posible de nuestra red.
        también es fácil de calcular:
        haciendo una operación OR sobre netid y la dirección de wildcard
	 */  
	netdata->broadcast.s_addr = netdata->netid.s_addr | netdata->wildcard.s_addr; 
	/*
	    netid + 1 calcula el primer host utilizable en nuestra red
        si nuestro netid es 192.168.10.0 y nuestra máscara de subred es 255.255.255.0
        entonces nuestro primer host sería 192.168.10.1
        Pero en este punto tenemos un problema. Tenemos que ocuparnos de little-endian
        y big endian.
        Tenemos una dirección de 32 bits. Supongamos que nuestro netid es 192.168.10.0.
        entonces netdata-> netid.s_addr tendría el valor decimal 698560.
        eso está en binario:    00000000000010101010100011000000
        0 está en binario:      00000000 <-primero
        10 está en binario:             00001010
        168 está en binario:                    10101000
        192 está en binario:                            11000000 <- último
        por lo tanto, si solo aumentamos nuestro netid.s_addr en uno, obtendríamos:
        193.168.10.0
        en lugar de 192.168.10.1
        Es por eso que tenemos que cambiar el orden de los bytes, aumentar los 32 bits en 1
        y luego cambiar el orden de bytes nuevamente. Nuestra dirección se almacena en orden de bytes de host,
        entonces tenemos que usar la función htonl () para cambiarlo a orden de bytes de red. Si está en
        orden de bytes de red podemos aumentarlo y luego usamos ntohl () para convertirlo de nuevo
        al orden de bytes de host:
	 */  
	netdata->hostmin.s_addr = ntohl(htonl(netdata->netid.s_addr) +1);
	/* 
	    La última IP de cada red es la dirección de difusión. Entonces el
        La última IP utilizable de nuestra red es broadcast-address-1. como arriba
        tenemos que cambiar nuestra dirección a orden de bytes de red, disminuirla y
        cambiar de nuevo a host-byte-order nuevamente:
	 */
	netdata->hostmax.s_addr = ntohl(htonl(netdata->broadcast.s_addr) -1);
	/*
	    hosts almacenarán la cantidad máxima de ips utilizables en nuestra red.
        Se calcula con la wildcard -1. Aquí no almacenamos una dirección de red
        Esto es solo un valor entero que almacena el número de hosts, es por eso que
        convertimos nuestra dirección wildcard en un byte de red, la disminuimos y almacenamos
        este valor en hosts.
        En nuestro ejemplo, el comodín es 0.0.0.255 que está en decimal 255,
        255 disminuido en uno sería 254. por lo que podemos usar 254 ips en nuestra red
	 */  
	netdata->hosts = htonl(netdata->wildcard.s_addr)-1;
			
	return 0;
}

/*
    getCidr () calcula la máscara de subred y la devuelve e
    en notación cidr. Por ejemplo: 255.255.252.0 devuelve 22
    255.255.255.0 devuelve 24
 */  
int getCidr(struct in_addr *nm)
{
	int cidr = 0;
	struct in_addr netmask;
	netmask.s_addr = nm->s_addr;

	while ( netmask.s_addr )
	{
    		cidr += ( netmask.s_addr & 0x01 );
    		netmask.s_addr >>= 1;
	}

	return cidr;
}


int main(int argc, char *argv[])
{
	/*  esta es nuestra estructura de red
        va a ser llenado por calcNet
	 */
	ipnet netdata;
	if(argc != 3)
	{
		fprintf(stderr,"usage: %s <ip> <subnetmask>\n",argv[0]);
		return EXIT_FAILURE;
	}
	/* inet_pton convierte nuestra cadena de IP en una estructura de dirección de 32 bits */
	if(inet_pton(AF_INET,argv[1],&netdata.netid) == 1)
	{
		/* inet_pton convierte la cadena de máscara de subred en una estructura de dirección de 32 bits */
		if(inet_pton(AF_INET,argv[2],&netdata.netmask) == 1)
		{
			/* se calcula la red */
			calcNet(&netdata);

			/* se imprimen todos los miembros de la estructura */
			printf("Network:\t%s/%d\n",inet_ntoa(netdata.netid),getCidr(&netdata.netmask));
			printf("Wildcard:\t%s\n",inet_ntoa(netdata.wildcard));
			printf("Netmask:\t%s\n",inet_ntoa(netdata.netmask));
			printf("Hostmin:\t%s\n",inet_ntoa(netdata.hostmin));
			printf("Hostmax:\t%s\n",inet_ntoa(netdata.hostmax));
			printf("Broadcast:\t%s\n",inet_ntoa(netdata.broadcast));
			printf("Hosts:\t\t%u\n",netdata.hosts);

		}

	}

	return 0;
}