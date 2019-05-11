pathToFormat="."
if [[ ${#} == 1 ]]; then
  pathToFormat="${1}"
  if [[ ! -d "${pathToFormat}" ]]; then
    echo "ERROR: Path \"${pathToFormat}\" does not exist"
    exit
  fi
fi

ExtensionsToFormat=(
  C
  cpp
  cxx
  h
  hpp
)

cd $(pwd)
for ext in ${ExtensionsToFormat[@]}; do
  clang-format -i $(find ${pathToFormat} -type f -iname "*.${ext}")
done
cd - > /dev/null