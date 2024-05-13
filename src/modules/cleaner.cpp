/*

Need to take an input image and clean it up before getting each character.

At an abstract level it is a matter of separating the foreground from the
background.

The foreground is the text and the background is everything else.

Some assumptions:
- The background is mostly uniform.
- The text is mostly uniform (each character is mostly the same color)
- The background will have lines or a grid pattern (possibly NO pattern is the
easiest case)
- The background will have noise (dust, scratches, etc)
- There may be scratched out text
- The background may include patterns or sketches that are not crucial to the
image



*/

namespace Mathic {

class Cleaner {
public:
private:
};

} // namespace Mathic
