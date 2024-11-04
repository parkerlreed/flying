# flying
Flying Pool/snooker/billiards/carrom/etc game

Now for modern Linux!

Game requires 8bpp Xorg so we use Xephyr to run in a nested window (Works for both Wayland and X11)

```
make -f Makefile.Linux
Xephyr :15 -screen 1024x768x8 -ac &
DISPLAY=:15 ./flying -pool
```

Enjoy
