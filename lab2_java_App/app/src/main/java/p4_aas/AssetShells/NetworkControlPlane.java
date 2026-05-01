// Copyright 2023 riccardo.bacca@studio.unibo.it
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package p4_aas.AssetShells;

import org.eclipse.basyx.aas.metamodel.api.parts.asset.AssetKind;

import p4_aas.Submodels.Controller.Controller;

/**
 * Extends AbstractShell for creating a Network Control Plane AAS
 * This AAS represetns the Control Plane in a Network Configuration,
 * allowing to exchange informations with Controllers and Firewalls
 */
public class NetworkControlPlane extends AbstractShell {

    public NetworkControlPlane(Integer PORT, String idShort, String pathId, AssetKind kind) {
        super(PORT);

        this.shell = this.createShell(idShort, pathId, kind);
        this.createSubmodels();
    }

    private void createSubmodels() {
        this.submodels.addAll(new Controller(1).createSubmodel());
        this.submodels.addAll(new Controller(2).createSubmodel());
    }
}