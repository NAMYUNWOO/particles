#!/bin/bash
# Web deployment script for GitHub Pages
# This script builds the WebAssembly version and deploys to gh-pages branch

set -e  # Exit on error

echo "🚀 Starting web deployment..."

# 1. Clean and build
echo "📦 Building WebAssembly version..."
make -f Makefile.web clean
make -f Makefile.web

# 2. Check if build succeeded
if [ ! -f "build/web/particle_storm.html" ]; then
    echo "❌ Build failed! particle_storm.html not found."
    exit 1
fi

echo "✅ Build complete!"

# 3. Copy build files to temp directory
echo "📦 Copying build files to temp directory..."
TEMP_DIR=$(mktemp -d)
cp build/web/* "$TEMP_DIR/"

# 4. Save current branch
CURRENT_BRANCH=$(git branch --show-current)
echo "📌 Current branch: $CURRENT_BRANCH"

# 5. Checkout gh-pages (create if doesn't exist)
if git show-ref --verify --quiet refs/heads/gh-pages; then
    echo "🔀 Switching to gh-pages branch..."
    git checkout gh-pages
else
    echo "🆕 Creating new gh-pages branch..."
    git checkout --orphan gh-pages
fi

# 6. Remove all files except .git
echo "🧹 Cleaning gh-pages branch..."
git rm -rf . 2>/dev/null || true
rm -rf * 2>/dev/null || true

# 7. Copy build output from temp to root
echo "📋 Copying build files from temp..."
cp "$TEMP_DIR/particle_storm.html" .
cp "$TEMP_DIR/particle_storm.js" .
cp "$TEMP_DIR/particle_storm.wasm" .

# 8. Clean up temp directory
rm -rf "$TEMP_DIR"

# 9. Create .nojekyll to disable Jekyll processing
touch .nojekyll

# 10. Create simple index.html redirect
cat > index.html << 'EOL'
<!DOCTYPE html>
<html>
<head>
    <meta http-equiv="refresh" content="0; url=particle_storm.html">
    <title>Particle Storm</title>
</head>
<body>
    <p>Redirecting to <a href="particle_storm.html">Particle Storm</a>...</p>
</body>
</html>
EOL

# 11. Commit and push
echo "💾 Committing changes..."
git add .
git commit -m "Deploy web build $(date '+%Y-%m-%d %H:%M:%S')" || echo "No changes to commit"

echo "📤 Pushing to GitHub..."
git push origin gh-pages --force

# 12. Return to original branch
echo "🔙 Returning to $CURRENT_BRANCH branch..."
git checkout "$CURRENT_BRANCH"

echo ""
echo "✅ Deployment complete!"
echo ""
echo "🌐 Your game will be available at:"
echo "   https://namyunwoo.github.io/particles/"
echo ""
echo "⏱️  It may take a few minutes for GitHub Pages to update."
