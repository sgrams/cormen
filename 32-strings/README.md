# String matching algorithms
Dependencies:
1. `glib-2.0`

Compile with command \
`make`

Syntax \
`./main`

---
| Algorithm          | Preprocessing time | Matching time     |
| ------------------ | ------------------ | ----------------- |
| Naive              | 0                  | *O((n – m + 1)m)* |
| Rabin-Karp         | Θ(m)               | *O((n – m + 1)m)* |
| Finite automation  | O(m\|Σ\|)          | *Θ(n)*            |
| Knuth-Morris-Pratt | Θ(m)               | *Θ(m)*            |
---
Tested under kernel 4.16.9-1-ARCH and gcc 8.1.0.
\
This file is a part of 32-strings task.

README.md\
*Stanisław J. Grams*