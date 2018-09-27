% Dans les entrailles d'un débogueur
% Camille Oudot (SOFT/LANNION)

# Introduction

- - -

```bash
bash$ gcc -g hello_world.c -o hello
bash$ ./hello
    Hello Dev Days 2018.
    Debugging is not that hard...
bash$
```

## hello\_world.c

```c
int main(int argc, char *argv[]) {
    print_letter('H');
    print_letter('e');
    print_letter('l');
    print_letter('l');
    print_letter('o');
    (...)
    print_letter(' ');
    print_letter('i');
    print_letter('s'); /* line 41 */
    print_letter(' '); /* line 42 */
    print_letter('n'); /* line 43 */
	(...)
}
```

- - -

```
bash$ gdb hello

(gdb)












```

- - -

```
bash$ gdb hello

(gdb) break hello_world.c:42
Breakpoint 1 at 0x400721: file hello_world.c, line 42.

(gdb)









```
- - -

```
bash$ gdb hello

(gdb) break hello_world.c:42
Breakpoint 1 at 0x400721: file hello_world.c, line 42.

(gdb) run
    Hello Dev Days 2018.
    Debugging is
Breakpoint 1, main (...) at hello_world.c:42

(gdb)




```
- - -

```
bash$ gdb hello

(gdb) break hello_world.c:42
Breakpoint 1 at 0x400721: file hello_world.c, line 42.

(gdb) run
    Hello Dev Days 2018.
    Debugging is
Breakpoint 1, main (...) at hello_world.c:42

(gdb) continue
Continuing.
     not that hard...
[Inferior 1 (process 24925) exited normally]
```


# Que s'est-il passé ?

- - -
> - `break hello_world.c:42`
>     - le débogueur a déterminé l'adresse du code machine de la ligne 42 dans la mémoire du processus tracé
>     - il a écrasé ce code pour y placer un code qui fait planter le programme tracé
> - `run`
>     - il a démarré le programme tracé et attendu qu'il plante
>     - il a restoré le code machine d'origine du programme tracé
> - `continue`
>     - il a fait repartir le programme tracé

# Trouver l'adresse du code machine de la ligne 42

# ELF, DWARF ?



