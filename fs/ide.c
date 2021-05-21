/*
 * operations on IDE disk.
 */

#include "fs.h"
#include "lib.h"
#include <mmu.h>

static void write_dev_checked(void *va, u_int dev, u_int len) {
	if (syscall_write_dev((u_int)va, dev, len) < 0) {
		user_panic("ide: could not write to disk controller");
	}
}

static void read_dev_checked(void *va, u_int dev, u_int len) {
	if (syscall_read_dev((u_int)va, dev, len) < 0) {
		user_panic("ide: could not read from disk controller");
	}
}

// Overview:
// 	read data from IDE disk. First issue a read request through
// 	disk register and then copy data from disk buffer
// 	(512 bytes, a sector) to destination array.
//
// Parameters:
//	diskno: disk number.
// 	secno: start sector number.
// 	dst: destination for data read from IDE disk.
// 	nsecs: the number of sectors to read.
//
// Post-Condition:
// 	If error occurred during read the IDE disk, panic. 
// 	
// Hint: use syscalls to access device registers and buffers
void
ide_read(u_int diskno, u_int secno, void *dst, u_int nsecs)
{
	static u_int op = 0;
	u_int i, off, stat;

	off = secno * BY2SECT;
	for (i = 0; i < nsecs; i++) {
		write_dev_checked(&off, DISK_ADDR_OFF, 4);
		write_dev_checked(&diskno, DISK_ADDR_ID, 4);
		write_dev_checked(&op, DISK_ADDR_OP, 4);
		read_dev_checked(&stat, DISK_ADDR_STAT, 4);
		if (!stat) {
			user_panic("ide_read: could not read from disk");
		}
		read_dev_checked(dst, DISK_ADDR_BUF, BY2SECT);
		off += BY2SECT;
		dst += BY2SECT;
	}
}

// Overview:
// 	write data to IDE disk.
//
// Parameters:
//	diskno: disk number.
//	secno: start sector number.
// 	src: the source data to write into IDE disk.
//	nsecs: the number of sectors to write.
//
// Post-Condition:
//	If error occurred during read the IDE disk, panic.
//	
// Hint: use syscalls to access device registers and buffers
void
ide_write(u_int diskno, u_int secno, void *src, u_int nsecs)
{
	static u_int op = 1;
	u_int i, off, stat;

	// DO NOT DELETE WRITEF !!!
	writef("diskno: %d\n", diskno);

	off = secno * BY2SECT;
	for (i = 0; i < nsecs; i++) {
		write_dev_checked(&off, DISK_ADDR_OFF, 4);
		write_dev_checked(&diskno, DISK_ADDR_ID, 4);
		write_dev_checked(src, DISK_ADDR_BUF, BY2SECT);
		write_dev_checked(&op, DISK_ADDR_OP, 4);
		read_dev_checked(&stat, DISK_ADDR_STAT, 4);
		if (!stat) {
			user_panic("ide_write: could not write to disk");
		}
		off += BY2SECT;
		src += BY2SECT;
	}
}

