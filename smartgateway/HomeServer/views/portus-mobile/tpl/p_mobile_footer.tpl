  <!-- BEGIN COPYRIGHT -->
  <div class="copyright">
    2016 Copyright © All Rights Reserved Powered By Portus
  </div>
  <!-- END COPYRIGHT -->
</body>
<!-- END BODY -->
</html>


<script type="text/javascript">
    var sock = null;
  var wsuri = "ws://127.0.0.1:8360/ws";
  $(document).ready(function(){
    $.post("/ws",{
      url:wsuri,
    },
    function(e,status){
      //alert("数据: \n" + data  +"\n状态: " + status); 
      wsuri = e.data; 
      //alert(wsuri)  
    });
    });

    window.onload = function() {  
        console.log("onload");
        sock = new WebSocket(wsuri);
        sock.onopen = function() {
            console.log("connected to " + wsuri);
        }
        sock.onclose = function(e) {
            console.log("connection closed (" + e.code + ")");
        }
        sock.onmessage = function(e) {
            console.log("message received: " + e.data);
      alert(e.data)
        }
    };

    function send() {
        var msg = document.getElementById('message').value;
        sock.send(msg);
    }

</script>