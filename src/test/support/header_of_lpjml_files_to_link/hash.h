Hash newhash(int,int (*)(const void *,int),int (*)(const void *,const void *),
                    void (*)(void *));
int addhashitem(Hash,void *,void *);
Bool removehashitem(Hash,const void *);
int gethashcount(const Hash);
void *gethashitem(Hash,const void *);
Hashitem *hash2array(const Hash);
void deletehash(Hash);
void freehash(Hash);
