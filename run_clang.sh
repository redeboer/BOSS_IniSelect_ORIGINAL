ExtensionsToFormat=(
  C
  cpp
  cxx
  h
)

cd $(pwd)
for ext in ${ExtensionsToFormat[@]}; do
  clang-format -i $(find . -type f -iname "*.${ext}")
done
cd - > /dev/null