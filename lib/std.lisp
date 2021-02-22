; boolean types
(letc {true} 1)
(letc {false} 0)

; aliases
(letc {+} add)
(letc {-} sub)
(letc {*} mul)
(letc {/} div)

; function definition
(letc {fn}
  (lambda {args body}
    {let (head args) (lambda (tail args) body)}))

; currying
(letc {unpack}
  (lambda {f args} {eval (join (list f) args)}))

(letc {curry} unpack)

(letc {pack}
  (lambda {f & xs} {f xs}))

(letc {uncurry} pack)
