for file in $(ls *.xsd | grep -v '^xml\.xsd$'); do
  initial=${file:0:1}
  remainder=${file:1}
  echo "void parse${initial^^}${remainder%.xsd}_data();"
  echo "void parse${initial^^}${remainder%.xsd}();"
  sed -Ene 's|.*xsd:complexType +name="([^"]+)".*|\nvoid parse'"${initial^^}${remainder%.xsd}"'_\1_data();\nvoid parse'"${initial^^}${remainder%.xsd}"'_\1();|p' "${file}"
  echo
done
