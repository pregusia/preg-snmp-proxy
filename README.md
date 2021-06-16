# preg-snmp-proxy
SNMP Proxy allowing for collecting statistics and caching OIDS data


Example config
==========================

```
proxy {

	community "..." "...";
	socket "0.0.0.0:161";

	target {
		src-socket "192.168.1.1:12345";
		dest-socket "1.1.1.1:161";
		community "...";
	};

	statistics {
		file "/some/stats/file.txt";
		write-interval 60;
	};

	
	cache-for ".1.3.6.1.2.1.2.2.1.17.*" {
		update-interval 60;
	};
	cache-for ".1.3.6.1.2.1.2.2.1.18.*" {
		update-interval 60;
	};
	

};

proxy {
	
	...
	
	
};
```


Configuration file description
========

1. Config file is block language. Each entry start with name, optional parameters and ends with ';'.
2. proxy entry -> represents one proxy instance
3. proxy.community -> represents accepted communities on this proxy. Can be many.
4. proxy.socket -> server socket for this proxy
5. proxy.target -> destination system for this proxy instance
6. proxy.target.src-socket -> source socket from which packets will be send to this target
7. proxy.target.dst-socket -> destination device endpoint
8. proxy.target.src-socket -> destination device community
9. proxy.statistics -> statistics collector for this proxy
10. proxy.statistics.file -> statistics output file
11. proxy.statistics.write-interval -> statistics dump interval
12. proxy.cache-for -> specifies base OID which shall be cached. For cached OIDS get-bulk is performed each 'update-interval'. And queries for this OIDS (or its children) will be returned from cache instead of target system.



