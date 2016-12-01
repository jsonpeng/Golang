<script type="text/javascript">
function screenAdapter(){
    document.getElementById('footer').style.top=document.documentElement.scrollTop+document.documentElement.clientHeight- document.getElementById('footer').offsetHeight+"px";
        document.getElementById('navigator').style.height=document.documentElement.clientHeight-100+"px";
        document.getElementById('main').style.height=document.documentElement.clientHeight-100+"px";
       // document.getElementById('main').style.width=window.screen.width-870+"px";
        //document.getElementById('main').style.width=window.screen.width-230+"px";
}

window.onscroll=function(){screenAdapter();};
window.onresize=function(){screenAdapter();};
window.onload=function(){screenAdapter();};
</script>
</html>