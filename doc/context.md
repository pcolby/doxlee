# Context Variables

## Common

Context variables available to all templates:

### `doxleeVersion`

Version of [doxlee] currently processing the template, for example `1.0.0`.

### `doxygenVersion`

Version of [Doxygen] that generated the XML input, for example `1.9.1`.

### `doxygenLanguage`

Language specified in the [Doxygen] XML index, for example `en-US`.

### `compoundsList`

Compounds listed in the order they appear [Doxyen] XML index. Note, the 

* `compoundsList[].refid`
* `compoundsList[].kind`
* `compoundsList[].name`
* `compoundsList[].members[].refid`
* `compoundsList[].members[].kind`
* `compoundsList[].members[].name`

### `compoundsByKind`

The same compound information as `compoundsList`, but grouped by *kind* (eg, `class`, `struct`, `union`,
`file`, etc). Under *kind*, the compounds are sorted by *name*.

* `compoundsByKind[kind].refid`
* `compoundsByKind[kind].kind`
* `compoundsByKind[kind].name`
* `compoundsByKind[kind].members[].refid`
* `compoundsByKind[kind].members[].kind`
* `compoundsByKind[kind].members[].name`

### `compoundsByRefId`

The same compound information as `compoundsList`, but indexed by compound *refid*.

* `compoundsByRefId[refid].refid`
* `compoundsByRefId[refid].kind`
* `compoundsByRefId[refid].name`
* `compoundsByRefId[refid].members[].refid`
* `compoundsByRefId[refid].members[].kind`
* `compoundsByRefId[refid].members[].name`

### `membersByKind`

All members contained in `compoundsList`, grouped by *kind* (eg `function`, `variable`, `signal`, etc).

* `membersByKind[kind].refid`
* `membersByKind[kind].kind`
* `membersByKind[kind].name`

### `membersByRefId`

All members contained in `compoundsList`, indexed by member *refid*.

* `membersByRefId[kind].refid`
* `membersByRefId[kind].kind`
* `membersByRefId[kind].name`

## Per Compound

Context variables available to per-compound templates:

\todo

[Doxygen]: https://github.com/doxygen/doxygen
[doxlee]: https://github.com/pcolby/doxlee
