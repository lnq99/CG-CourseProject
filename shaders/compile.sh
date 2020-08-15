GL_COMPILER='./glslc'
OUT_DIR='../data/shaders'

$GL_COMPILER ray/texture.vert -o $OUT_DIR/texture.vert.spv
$GL_COMPILER ray/texture.frag -o $OUT_DIR/texture.frag.spv

$GL_COMPILER ray/raytracing.comp -o $OUT_DIR/raytracing.comp.spv

$GL_COMPILER ui/text.vert -o $OUT_DIR/text.vert.spv
$GL_COMPILER ui/text.frag -o $OUT_DIR/text.frag.spv

$GL_COMPILER ui/ui.vert -o $OUT_DIR/ui.vert.spv
$GL_COMPILER ui/ui.frag -o $OUT_DIR/ui.frag.spv