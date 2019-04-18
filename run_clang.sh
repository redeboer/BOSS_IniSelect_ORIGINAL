ExtensionsToFormat=(
  C
  cpp
  cxx
  h
)

cd $(pwd)
for ext in ${ExtensionsToFormat[@]}; do
  echo "Applying clang-format to \"${ext}\" files..."
  { clang-format -i $(find . -type f -iname "*.${ext}" | grep -ve "^./extLibs"); } &> /dev/null
done
cd - > /dev/null