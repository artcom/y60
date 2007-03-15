
function Patch() {
        this.Constructor(this);
}

Patch.prototype.Constructor = function(self) {
        
        self.onSceneLoaded = function() {
        	// use window.scene.world...
        }
        
        self.onFrame = function(theTime) {
        	
        }  
}