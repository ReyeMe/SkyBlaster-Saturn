# SkyBlaster
Shmup game for sega saturn
Made for SEGA Saturn 27th Anniversary Game Competition

Made with jo-engine
Proudly using PoneSound for audio

# Credits
Code and art by me
Music by RandomStuff
More art by AnriFox

# Some notes for if you want to compile this your self
To compile this with jo-engine correctly first K_LINECOL flag must be removed from jo_vdp2_set_rbg0_plane_a_8bits_image
Secondly line 276 in jo_engine_makefile must be changed to TARGET1  = $(TARGET:.elf=.raw) from TARGET1  = $(TARGET:.elf=.bin) to prevent garbage track 2
