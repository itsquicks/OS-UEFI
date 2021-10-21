cls
@pushd %~dp0

qemu-system-x86_64 -drive format=raw,file=bin\CustomOS.img -m 256M -cpu qemu64 -drive if=pflash,format=raw,unit=0,file=..\OVMFbin\OVMF_CODE-pure-efi.fd,readonly=on -drive if=pflash,format=raw,unit=1,file=..\OVMFbin\OVMF_VARS-pure-efi.fd -net none -hdb C:\Users\Fabian\Desktop\ext2drive.iso

@popd