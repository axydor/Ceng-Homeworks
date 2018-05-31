dd if=/dev/zero of=image.img bs=1024 count=850
mke2fs -N 32 image.img
fuseext2 -o rw+ image.img mnt/
cp big mnt/
rm mnt/big
