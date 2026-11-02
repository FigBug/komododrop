#!/bin/bash -e

# Tags the current VERSION and pushes it, which kicks off the release workflow.

ROOT=$(cd "$(dirname "$0")"; pwd)
cd "$ROOT"

VERSION=$(cat VERSION)

if ! grep -q "^## $VERSION\$" Changelist.txt; then
  echo "ERROR: Changelist.txt has no '## $VERSION' section"
  exit 1
fi

git tag -a "$VERSION" -m "KomodoDrop $VERSION"
git push origin "$VERSION"
