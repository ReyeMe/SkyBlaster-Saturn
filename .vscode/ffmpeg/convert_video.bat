@echo off
set filename=%1

@set vq=16
@set fps=30/1
@set vproc="pp=ac,pp=ac,pp=ac,hqdn3d=5,eq=contrast=256/220:brightness=1/512:saturation=256/224:gamma=16/16,scale=320:-4:sws_flags=spline+accurate_rnd+full_chroma_int+full_chroma_inp,minterpolate=fps=%fps%:mi_mode=mci:me_mode=bidir:me=ntss:vsbmc=1,xbr=2,scale=320:-4:sws_flags=spline+accurate_rnd+full_chroma_int+full_chroma_inp:sws_dither=2:in_range=0:out_range=2,format=pix_fmts=rgb24,pad=320:224:(ow-iw)/2:(oh-ih)/2"
@set aproc="pan=mono|FC < 1.414FC+FR+FL+0.5BL+0.5SL+0.25LFE+0.125BR,firequalizer=gain='if(gte(f,25),0,-INF)+if(lte(f,11024),0,-INF)',dynaudnorm=p=1/sqrt(2):m=100:s=20,firequalizer=gain='if(gte(f,25),0,-INF)+if(lte(f,11024),0,-INF)',aresample=resampler=soxr:osr=22050:cutoff=0.990:dither_method=shibata"


for /R ".\out" %%f in (*.mov) do (
    del /Q /F ".\out\%%~nf.mov" > NUL
)

for /R ".\in" %%f in (*.mp4) do (
	.\ffmpeg -hide_banner -v 32 -stats -y -i ".\in\%%~nf.mp4" -c:v cinepak -c:a pcm_s8 -ar 32000 -filter:v "scale=w=320:h=224" -f mov ".\out\%%~nf.mov"
)

pause