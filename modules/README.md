# Description

To make styling easier, I created this module to abstract the escape codes and
macros with functions that wrap 'printf' and 'puts' and that use a syntax close
to strings interpolation to apply the styles.

    Possible styles:   
      B -> Bold
      D -> Dim
      I -> Italic
      U -> Underline
      K -> Blinking
      R -> Reverse
      H -> Hidden
      S -> Strikethrough
      P -> Double(Paired) Underline
      C -> Curly Underline
      O -> Overline

    Possible colors:   
      fd  -> black(dark)
      fr  -> red
      fg  -> green
      fy  -> yellow
      fb  -> blue
      fm  -> magenta
      fc  -> cyan
      fw  -> white

    For the background are the same,
    but with 'b' prefix.
