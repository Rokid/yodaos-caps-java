jar_path=out/bin
jni_path=out/lib
java -cp $jar_path/caps.jar:$jar_path/caps-exam.jar -Djava.library.path=$jni_path com.rokid.examples.CapsExample
