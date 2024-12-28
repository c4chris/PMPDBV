Poor Man's PDB Viewer

This is inspired by https://www.gradientspace.com/tutorials/2020/10/23/runtime-mesh-generation-in-ue426

The idea is to retrieve a PDB entry from https://www.rcsb.org/ (e.g. 1crn) and build a dynamic mesh with a small sphere for each atom using FDynamicMesh3 from Unreal Engine.

So far, I can get the entry and construct the mesh, but the Actor never updates the displayed mesh, neither in the Editor, not when running in Game mode.

I must be missing some code somewhere...

I asked for help here : https://forums.unrealengine.com/t/how-can-i-get-my-dynamicmeshactor-to-update-the-displayed-mesh-after-updating-the-fdynamicmesh3-component/2246652
