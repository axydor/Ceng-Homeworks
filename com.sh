dd if=/dev/zero of=image.img bs=1024 count=128
mke2fs -N 32 image.img
fuseext2 -o rw+ image.img mnt/
#cp i p mnt/
#rm mnt/i mnt/p
