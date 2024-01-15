
# DNS LIST SORTER
*This title might change later*

Hi, and thank you for looking at my DNS list sorter program.
Like a lot for self-hosters, I use private hosted DNS (ad-blocker), on my network, to limit some traffic heading towards ad sites and tracking sites. I usually just go to block lists, import them, and don't think about it after, but setting up those lists, usually involves adding whitelist to them, and then giving permits to special sites, so they can work as normal. 
Instead of doing this by adding the items to whitelist, I would rather just remove the items from the block-lists. Its probably the same, but this is the way I like to set it up.
***
*Disclaimer: This setup is for linux systems. I have not tested on windows/mac*
***
## How to install
```sh  
git clone https://git.benmi.me/benmi/dns_list_sorter.git
cd dns_list_sorter/build
cmake ..
make
```
## How to use
```sh  
cp ../config-example.toml config.toml
```
edit the config.toml, add the lists you want to it the category you want it in.( Category does not change anything, you could just add them all into say Other, it will also go into the fulllist.txt)
run the program
```sh  
./dns_list_sorter
```
## For any problems
Feel free to a pull request!
***
*PS: I will leave my config up as long is it does not create any problems. Please let me know if it creates any problems, and I will remove it.*
***

[![Licenわわse](LICENSE.md)]
