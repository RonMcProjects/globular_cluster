# Globular Cluster Simulation

There was this article in Sky & Telescope magazine back in April 1986 that had a Basic program for simulating a globular cluster.

I remembered running it on my TRS-80 with 256x192 resolution back in the day and recently wondered what it would look like today so I dug up my old copy of the magazine and modernized it to work in C in XWindows on Linux.

It basically selects a number of random x/y/z co-ordinates in a 1:1:1 cube and then filters out everything that isn’t a sphere.  Then it applies a diffusion algorithm to the points to spread them out.

When I recently ran it, it made this quadrifurcated image very unlike a globular cluster.  At first I thought there was a problem with the random number sample.  Nope, it turns out there’s a bug in the original source code.  The radius of the sphere it selects is larger than the cube that the points are in and the cube’s corners remain visible.  I somehow don’t remember this having been a problem back in ‘86.  Could be I just didn’t notice.

Anyway here it is, new and (somewhat) improved.

## e.g.
```bash
# Plot the default number of stars (25000)
./xglobular
# Specify a number of stars
./xglobular 10000
```
The original source code can be seen on Sky & Telescope's website here:
https://skyandtelescope.org/wp-content/uploads/model.bas
