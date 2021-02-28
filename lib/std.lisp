; ---------------
; PRIMITIVE TYPES
; ---------------

; boolean types
(globalc {true} 1)
(globalc {false} 0)

; empty (nil)
(globalc {empty} {})


; -------
; ALIASES
; -------

; math operations
(globalc {+} add)
(globalc {-} sub)
(globalc {*} mul)
(globalc {/} div)

; conditionals
(globalc {==} eq)
(globalc {>}  gt)
(globalc {>=} ge)
(globalc {<}  lt)
(globalc {<=} le)
(globalc {!=} ne)


; ----------------
; FUNCTION HELPERS
; ----------------

; function definition
(globalc {fn}
  (lambda {args body}
    { global (head args) (lambda (tail args) body) }))

; curry
(fn {unpack f args} { eval (join (list f) args) })
(globalc {curry} unpack)

; uncurry
(fn {pack f & xs} { f xs })
(globalc {uncurry} pack)

; compose the result of (g x) to f
(fn {comp f g x} {f (g x)})


; -----------------
; LOGICAL OPERATORS
; -----------------

; NOT gate
(fn {not x}
  { if (or (== x true) (== x false))
    { - true x }
    { false } })

; OR gate
(fn {or x y}
  { if (>= (+ x y) 1)
    { true }
    { false } })

; AND gate
(fn {and x y}
  { if (>= (* x y) 1)
    { true }
    { false } })

; NAND (NOT AND) gate
(fn {nand x y} { not (and x y) })

; NOR (NOT OR) gate
(fn {nor x y}  { not (or x y) })

; XOR (exclusive OR) gate
(fn {xor x y}  { or (and x (not y)) (and (not x) y) })

; XNOR (NOT exclusive OR) gate
(fn {xnor x y} { not (xor x y) })


; --------------
; LIST FUNCTIONS
; --------------

; get the length of a list
(fn {len-of l}
  { if (== l empty)
    { 0 }
    { + 1 (len-of (tail l)) } })

; get the Nth value of a list
(fn {nth-of l n}
  { if (<= n 0)
    { first-of l }
    { nth-of (tail l) (- n 1) } })

; first value of a list
(fn {first-of l}  { eval (head l) })

; second valus of a list
(fn {second-of l} { eval (head (tail l)) })

; third value of a list
(fn {third-of l}  { eval (head (tail (tail l))) })

; last value of a list
(fn {last-of l}   { nth-of l (- (len-of l) 1) })

; take the first N values of a list
(fn {take-start-of l n}
  { if (== n 0)
    { empty }
    { join (head l) (take-start-of (tail l) (- n 1)) } })

; drop the first N values of a list
(fn {drop-start-of l n}
  { if (== n 0)
    { l }
    { drop-start-of (tail l) (- n 1) } })
