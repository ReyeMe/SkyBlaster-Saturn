![SkyBlaster logo](https://reye.me/img/projects-other/skyblaster_logo.png)
# SkyBlaster
Shmup game for sega saturn  
Made for [SEGA Saturn 27th Anniversary Game Competition](https://segaxtreme.net/threads/sega-saturn-27th-anniversary-game-competition-discussion-thread.25011/)  

Made with [jo-engine](https://jo-engine.org/)  
Proudly using [PoneSound](https://github.com/ponut64/SCSP_poneSound) for audio

## Credits
Code and art by me  
Music by RandomStuff  
More art by AnriFox  

## Some notes for if you want to compile this your self
 - To compile this with jo-engine correctly first ``K_LINECOL`` flag must be removed from ``jo_vdp2_set_rbg0_plane_a_8bits_image``  
 - Secondly line 276 in ``jo_engine_makefile`` must be changed to ``TARGET1 = $(TARGET:.elf=.raw)`` from ``TARGET1 = $(TARGET:.elf=.bin)``, this will prevent creation of garbage bin file assigned to track 2
