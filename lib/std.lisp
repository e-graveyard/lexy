; boolean types
(let {true} 1)
(let {false} 0)

; aliases
(let {+} add)
(let {-} sub)
(let {*} mul)
(let {/} div)

; function definition
(let {fn}
  (lambda {args body}
    {let (head args) (lambda (tail args) body)}))

; currying
(let {unpack}
  (lambda {f args} {eval (join (list f) args)}))

(let {curry} unpack)

(let {pack}
  (lambda {f & xs} {f xs}))

(let {uncurry} pack)
