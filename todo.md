*This is just a file to take some notes in. The name and everything else can change...*

- My initial idea was to have the blockchain fully connected but that is not the norm; therefore we should probably implement a P2P network model where there are "clusters" of connected nodes. I am not sure how this should correspond to the cluster head system though.
    - This entails implementing longest chain rules and such in order to resolve conflicts in the blockchain.
- Blockchain blocks have a transaction-data variable now. I think that should be made into a separate class/structure which accept new attributes as we add them. Thus we can decide to add new properties/information to the block as we want and perhaps assign them default values in order to keep backwards compatibility with our own tests :p
- Broadcast özelliğini araştıralım çünkü servis istekleri başlangıçta nasıl brodcast edilmeli emin değiliz.
- Trust hesaplamalarını eklemeliyiz.
- Yeni clusterlar eklemeliyiz. Cluster Head'leri birbirlerine bağlamamız lazım. Cluster headler sabit kalmayacağı için dinamik bir sisteme ihtiyacımız var.(bunu araştırmalıyız)
- Blokzincir tam bağlı olmayacağı için cluster’lar arasındaki iletişimde Djkstra gibi bir algoritma kullanılması gerek. 
    - Bunun için routing table’larla ilgili kısmı halleden bir send_message_to_neighbour() gibi bir metot yazılmalı
