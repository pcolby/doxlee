for file in $(ls *.xsd | grep -v '^xml\.xsd$'); do
  initial=${file:0:1}
  remainder=${file:1}
  echo "QVariantMap parse${initial^^}${remainder%.xsd} (QXmlStreamReader &xml) const;"
  sed -Ene 's|.*xsd:complexType +name="([^"]+)".*|QVariantMap parse'"${initial^^}${remainder%.xsd}"'_\1 (QXmlStreamReader \&xml) const;|p' "${file}"
  echo
done | column -Lto ' '
