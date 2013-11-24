QRT source and executable is Copyright 1988 and 1989 Steve Koren. It may
be freely copied, modified, and  distributed  under the following
conditions:

  1) Notice of any changes is posted and distributed with subsequent
     versions
  2) Neither the source nor executable is sold for profit.
  3) This message is distributed unmodified with the source and
     executable.

Any images created with QRT are yours to do with as you wish.

NOTE FOR AMIGA VERSION:

  Image creation involves 3 steps:
    QRT <Image.QRT >Image.out
    QRTPOST Image.RAW Image.TMP
    RAY2 Image

  The final IFF picture will have a filename of Image.ILBM.

  QRT and QRTPOST were written by Steve Koren.  RAY2 was written
  by D. Wecker and is Copyright 1987.  RAY2 creates a HAM image
  from a bitmap representation produced by QRTPOST.

  QRT should be run in the background with a low priority and
  its output file should be placed on a virtual disk.  It will
  produce 400K of output, so extended memory is recommended.

  Stack size must be increased from the system default of 4000
  bytes with the STACK command.  The exact size necessary varies
  with the image, but 50000 to 60000 bytes is usually safe.

  When QRT is running, you can check its progress by getting a
  directory of the device on which the Image.RAW file is being
  created.  A full image has 400 scan lines, and will be just
  short of 800 blocks when complete.  Thus, divide the blocks
  so far by 2 to discover how far into the image QRT has
  processed.  Different parts of an image might be created at
  different speeds.


Any comments, bug reports, etc, may be sent by USMAIL to:

     Steve Koren
     4501 Boardwalk Dr.
     Apt. D-38
     Fort Collins, CO 80525
     USA

or by email to:

     koren@hpfela.HP.COM

The USMAIL address is guarenteed to be correct until 01 JUL 89.


