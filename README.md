# locator

`locator` is meant to quickly generate indices of subsets of data. Observations of data are associated with labels, divided into categories, enabling the fast lookup of single labels and label combinations.

### dependencies (main library)
* c++14 compatible compiler.

### dependencies (matlab api)
* matlab >= r2015b.
* mex-compatible compiler (if the included mex files do not work for your platform).
* matlab utils `global` repository ([available here](https://github.com/nfagan/global)); only required for certain tests and example uses of the api.