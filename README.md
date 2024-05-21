# AGI
This repository contains about 75% of the original source code of Sierra On-Line's AGI adventure game interpreter, complete with comments, and, in some cases, a change history containing programmer initials.

These files were extracted from the unused parts of original Sierra AGI game disks, primarily from disk 1 of version 2.0D of Space Quest II (the 720KB version), and a few additional files from disk 1 of version 2.14 of King's Quest III (the 720KB version). Anyone who bought these versions of the games from Sierra On-Line unknowingly has a copy of the same original AGI source code files hiding on their disks. Back in the days when these games were best sellers for Sierra, there could very well have been tens of thousands, if not hundreds of thousands of people across the world who unknowingly had access to Sierra's AGI interpreter source code at their finger tips. Surprisingly, it wasn't until decades later that someone first noticed this.

The reason for the existence of AGI interpreter source code on those original game disks is due to the way in which the customer copies of disks were mass produced. Sierra used a disk duplication machine called a FormMaster that loaded the whole content of the master disk into memory. This involved loading every single byte of every sector of the master disk into memory, regardless of whether the sectors were currently being used on the master disk or not, i.e. it also copied the supposedly "free" space. Normally this wouldn't have mattered, assuming that Sierra fully formatted their master disks before preparing them for a release, but in the case of these SQ2 and KQ3 disks, they appear to have missed that step. Unfortunately for Sierra, these particular master floppy disks had previously been used to hold a copy of the AGI interpreter source code.

In those days, when a file was deleted from a DOS floppy disk, it did not completely erase the data, in fact all it did was mark those sectors as no longer being in use. The data itself remained on the disks. In the case of the SQ2 disk, over half of the disk was left unused after the files for version 2.0D of SQ2 were copied onto the disk. This meant that the data of the deleted files on that second half of the disk remained on there and was then copied to all of the customer copies.

It makes for a great digital archeology story. Due to this slip up, the source code has been preserved where it might otherwise have been lost forever. I don't think that its existence is commonly known of though, which is why I decided to create this repo and to spread the word.

For more detail regarding this blunder, you can read my blog post below: 

[The Space Quest II Master Disk Blunder](https://lanceewing.github.io/blog/sierra/agi/sq2/2024/05/10/do-you-own-this-space-quest-2-disk.html)

<br>

> [!NOTE]
> Recently, I released my own AGI interpreter called [AGILE](https://github.com/lanceewing/agile-gdx) that runs directly in a web browser. The original AGI interpreter source code was a great source of information to help make sure that my implementation was as accurate as possible. For those who are interested, you can try it out here: [https://agi.sierra.games/](https://agi.sierra.games/)
