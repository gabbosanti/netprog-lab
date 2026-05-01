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

import java.util.List;

import org.eclipse.basyx.aas.metamodel.api.parts.asset.AssetKind;
import org.eclipse.basyx.aas.metamodel.map.AssetAdministrationShell;
import org.eclipse.basyx.submodel.metamodel.map.Submodel;

import p4_aas.Infrastructure.ModelProvider;

/**
 * Interface for creating Asset Administration Shells
 */
public interface IShell {

    /**
     * @return void
     * Creates and start http server for each AAS
     */
    void createAndStartServlet();

    /**
     * @return interger Port to create HTTP server on
     */
    int getPORT();

    /**
     * @return created AAS
     */
    AssetAdministrationShell getShell();

    /**
     * @return ModelProvider for this specific AAS
     */
    ModelProvider getModelProvider();

    /**
     * @return list of submodels for this specific AAS
     */
    List<Submodel> getSubmodels();

    /**
     * Creates Descriptors (needed for ModelProvider) for each Submodel
     */
    void createDescriptors();

    /**
     * Add a submodel to the list
     */
    void addSubmodels();

    /**
     * @param idShort the simple id for AAS
     * @param pathId 
     * @param kind Instance or Type
     * @return a new AAS created with those parameters
     */
    AssetAdministrationShell createShell(String idShort, String pathId, AssetKind kind);

    /**
     * Creates model provider for this specifif AAS
     */
    void createModelProvider();

}